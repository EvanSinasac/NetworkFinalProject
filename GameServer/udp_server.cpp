#include "udp_server.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>
#include "globalThings.h"
#include <vector>


enum MessageType
{
	READY_TO_START = 1,
	DONT_START = 2,
	PLAYER_MOVEMENT = 3,
	PLAYER_NUMBER = 4,
	NON_PREDICTED_POSITION = 5,
};

struct Player {
	unsigned short port; // their id;
	struct sockaddr_in si_other;
	float x;
	float y;
	float z;
	bool up, down, right, left;
	bool ready;
	int playerNumber;
};

unsigned int numPlayersConnected = 0;

std::vector<Player> mPlayers;

//const float UPDATES_PER_SEC = 5;		// 5Hz / 200ms per update / 5 updates per second
std::clock_t curr;
std::clock_t prev;
double elapsed_secs;



void _PrintWSAError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

UDPServer::UDPServer(void)
	: mIsRunning(false)
	, mListenSocket(INVALID_SOCKET)
	, mAcceptSocket(INVALID_SOCKET)
	, UPDATES_PER_SEC(5)
{
	mPlayers.resize(4);

	// WinSock vars
	WSAData		WSAData;
	int			iResult;
	int			Port = 5155;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) {
		PrintWSAError();
		return;
	}

	// Step #1 Create a socket
	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET) {
		PrintWSAError();
		return;
	}

	// Step #2 Bind our socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}

	// Set our socket to be nonblocking
	SetNonBlocking(mListenSocket);

	// Our server is ready 
	printf("[SERVER] Receiving IP: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	mIsRunning = true;
	prev = std::clock();
} // end UDPServer

UDPServer::~UDPServer(void)
{
	closesocket(mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void UDPServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void UDPServer::Update(void)
{
	if (!mIsRunning) return;

	// TODO: ReadData, SendData
	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC) || numPlayersConnected < 4) return;
	prev = curr;

	// Might move this to the udp update, otherwise the positions need to be updated and sent each udp send
	::g_pWorld->TimeStep((float)elapsed_secs);

	UpdatePlayers();
	BroadcastUpdate();
}

void UDPServer::UpdatePlayers(void)
{
	/*for (int i = 0; i < numPlayersConnected; i++) {
		if (mPlayers[i].up) mPlayers[i].y += 10.0f * elapsed_secs;
		if (mPlayers[i].down) mPlayers[i].y -= 10.0f * elapsed_secs;
		if (mPlayers[i].right) mPlayers[i].x += 10.0f * elapsed_secs;
		if (mPlayers[i].left) mPlayers[i].x -= 10.0f * elapsed_secs;
	}*/
	// players are updated at every message, instead I need to update the particle positions
	


	for (unsigned int index = 1; index < ::g_pVecParticles.size(); index++)
	{
		for (unsigned int index2 = 0; index2 < mPlayers.size(); index2++)
		{
			if (::g_pVecParticles[index]->GetPlayer() == mPlayers[index2].playerNumber)
			{
				::g_pVecParticles[index]->SetPosition(glm::vec3(mPlayers[index2].x, mPlayers[index2].y, mPlayers[index2].z));
				break;
			}
		}
	}
}

void UDPServer::BroadcastUpdate(void)
{
	// And here, I want to update each of the player's positions as well as the ball's position
	// create our data to send, then send the same data to all players
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);


	// This goes through LG's player struct and then tells everyone what everyone's position is
	// Which isn't necessarily a bad way of handling it
	//memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));
	int mes = MessageType::PLAYER_MOVEMENT;
	memcpy(&(buffer[0]), &mes, sizeof(int));

	// Ball, need to send ball position as well

	for (int i = 0; i < ::g_pVecParticles.size(); ++i) {
		float x = ::g_pVecParticles[i]->GetPosition().x;//mPlayers[i].x;
		float y = ::g_pVecParticles[i]->GetPosition().y;//mPlayers[i].y;
		float z = ::g_pVecParticles[i]->GetPosition().z;//mPlayers[i].z;
		int playerNum = ::g_pVecParticles[i]->GetPlayer();// mPlayers[i].playerNumber;
		//printf("Particle values about to be sent: { %f %f %f %d }\n", x, y, z, playerNum);
		memcpy(&(buffer[i * 16 + 4]), &x, sizeof(float));
		memcpy(&(buffer[i * 16 + 8]), &y, sizeof(float));
		memcpy(&(buffer[i * 16 + 12]), &z, sizeof(float));
		memcpy(&(buffer[i * 16 + 16]), &playerNum, sizeof(int));
		//printf("Values in buffer: { %f %f %f %d }\n", buffer[i * 16 + 4], buffer[i * 16 + 8], buffer[i * 16 + 12], buffer[i * 16 + 16]);
	}

	// Dead reckoning needs the velocity of the ball
	float velX = ::g_pVecParticles[0]->GetVelocity().x;
	float velY = ::g_pVecParticles[0]->GetVelocity().y;
	float velZ = ::g_pVecParticles[0]->GetVelocity().z;
	memcpy(&(buffer[::g_pVecParticles.size() * 16 + 4]), &velX, sizeof(float));
	memcpy(&(buffer[::g_pVecParticles.size() * 16 + 8]), &velY, sizeof(float));
	memcpy(&(buffer[::g_pVecParticles.size() * 16 + 12]), &velZ, sizeof(float));

	//float bX, bY, bZ;
	//bX = ::g_pVecParticles[0]->GetPosition().x;
	//bY = ::g_pVecParticles[0]->GetPosition().y;
	//bZ = ::g_pVecParticles[0]->GetPosition().z;
	//int bNum = ::g_pVecParticles[0]->GetPlayer();	// which should be 0

	//memcpy(&(buffer[numPlayersConnected * 16 + 4]), &(bX), sizeof(float));
	//memcpy(&(buffer[numPlayersConnected * 16 + 8]), &(bX), sizeof(float));
	//memcpy(&(buffer[numPlayersConnected * 16 + 12]), &(bX), sizeof(float));
	//memcpy(&(buffer[numPlayersConnected * 16 + 16]), &bNum, sizeof(int));

	int packetLength = 12 + 4 + 16 * ::g_pVecParticles.size();
	for (int i = 0; i < numPlayersConnected; ++i)
	{
		int result = sendto(mListenSocket, buffer, packetLength, 0,
			(struct sockaddr*)&(mPlayers[i].si_other), sizeof(mPlayers[i].si_other));
	}
}

void UDPServer::BroadcastStartMessage(void)
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);
	uint32_t mes = MessageType::READY_TO_START;

	//memcpy(&(buffer[0]), &numPlayersConnected, sizeof(uint32_t));
	memcpy(&buffer[0], &mes, sizeof(uint32_t));
	int packetLength = sizeof(uint32_t);
	for (int i = 0; i < numPlayersConnected; i++)
	{
		int result = sendto(mListenSocket, buffer, packetLength, 0,
			(struct sockaddr*)&(mPlayers[i].si_other), sizeof(mPlayers[i].si_other));
	}
}

void UDPServer::BroadCastDontStartMessage(void)
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);
	uint32_t mes = MessageType::DONT_START;

	memcpy(&buffer[0], &mes, sizeof(uint32_t));
	int packetLength = sizeof(uint32_t);
	for (int i = 0; i < numPlayersConnected; i++)
	{
		int result = sendto(mListenSocket, buffer, packetLength, 0,
			(struct sockaddr*)&(mPlayers[i].si_other), sizeof(mPlayers[i].si_other));
	}
}

Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < mPlayers.size(); i++) {
		if (mPlayers[i].port == port) return &(mPlayers[i]);
	}

	// Otherwise create a new player, and return that one!
	mPlayers[numPlayersConnected].port = port;
	if (numPlayersConnected == 0)
	{
		mPlayers[numPlayersConnected].x = 10.0f;
		mPlayers[numPlayersConnected].y = 0.0f;
	}
	else if (numPlayersConnected == 1)
	{
		mPlayers[numPlayersConnected].x = -10.0f;
		mPlayers[numPlayersConnected].y = 0.0f;
	}
	else if (numPlayersConnected == 2)
	{
		mPlayers[numPlayersConnected].x = 0.0f;
		mPlayers[numPlayersConnected].y = 5.0f;
	}
	else if (numPlayersConnected == 3)
	{
		mPlayers[numPlayersConnected].x = 0.0f;
		mPlayers[numPlayersConnected].y = -5.0f;
	}
	mPlayers[numPlayersConnected].z = 0.0f;
	mPlayers[numPlayersConnected].si_other = si_other;
	mPlayers[numPlayersConnected].ready = false;
	mPlayers[numPlayersConnected].playerNumber = numPlayersConnected + 1;
	return &(mPlayers[numPlayersConnected++]);
}

void UDPServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mListenSocket, buffer, 512, 0, (struct sockaddr*)&si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// printf(".");		// Quick test
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}
	unsigned short port = si_other.sin_port;
	Player* player = GetPlayerByPort(port, si_other);
	// So this is where I need to parse the data sent by the client
	uint32_t messageID = buffer[0];
	//printf("Message ID: %d\n", messageID);
	switch (messageID)
	{
	case MessageType::READY_TO_START:
	{
		SendPlayerID(si_other);
		// When the player is ready to start, we have to go through and make sure everyone is ready to start.
		// Once all 4 players are ready, then the game can begin
		
		// This message should be sent when the player presses the Enter key at the beginning of the game
		//uint32_t ready = buffer[1];
		player->ready = true;
		// There's no point in telling the lobby they can start if there's not enough players
		bool areWeReady = false;
		if (numPlayersConnected == 4)
		{
			areWeReady = true;
			for (unsigned int index = 0; index < numPlayersConnected; index++)
			{
				areWeReady &= mPlayers[index].ready;
			}
		}
		// if areWeReady is true then we can broadcast the start message to the players
		if (areWeReady)
		{
			BroadcastStartMessage();
			::g_pVecParticles[0]->SetVelocity(glm::vec3(-10.0f, 1.0f, 0.0f));
		}
		else
		{
			BroadCastDontStartMessage();
		}

	}
	break;
	case MessageType::PLAYER_MOVEMENT:
	{
		// This will be the most common message, every update we will need to go through and parse each player's movement
		// This will be similar to the ball movement LG had
		// Client has moved (or tried to move) and the server needs to move the position of the particle to match
		// So index 0 is the message type, then the next 3 values should be the player's position in world space
		// And we are just updating the position of the particles here
		//sizeof(uint32_t) = 4UI64
		//sizeof(char) = 1UI64
		//sizeof(float) = 4UI64
		// MessageType should be an uint32_t at index 0, so add 4 to each one after?  I guess, idk
		float newX;// = buffer[4];
		float newY;// = buffer[8];
		float newZ;// = buffer[12];
		memcpy(&newX, &(buffer[4]), sizeof(float));
		memcpy(&newY, &(buffer[8]), sizeof(float));
		memcpy(&newZ, &(buffer[12]), sizeof(float));
		player->x = newX;
		player->y = newY;
		player->z = newZ;
		::g_pVecParticles[player->playerNumber]->SetPosition(glm::vec3(player->x, player->y, player->z));

		//printf("%d : %hu received position { %f %f %f }\n", mListenSocket,
		//	port, player->x, player->y, player->z);

	}
	break;
	case MessageType::NON_PREDICTED_POSITION:
	{
		float newX;// = buffer[4];
		float newY;// = buffer[8];
		float newZ;// = buffer[12];
		memcpy(&newX, &(buffer[4]), sizeof(float));
		memcpy(&newY, &(buffer[8]), sizeof(float));
		memcpy(&newZ, &(buffer[12]), sizeof(float));
		player->x = newX;
		player->y = newY;
		player->z = newZ;
		::g_pVecParticles[player->playerNumber]->SetPosition(glm::vec3(player->x, player->y, player->z));
	}
	break;
	default:
	{
		
	}
	}


	// THEN I can do something similar to update positions here.
	/*unsigned short port = si_other.sin_port;

	Player* player = GetPlayerByPort(port, si_other);

	player->up = buffer[0] == 1;
	player->down = buffer[1] == 1;
	player->right = buffer[2] == 1;
	player->left = buffer[3] == 1;

	printf("%d : %hu received { %d %d %d %d }\n", mListenSocket,
		port, player->up, player->down, player->right, player->left);*/

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}

void UDPServer::SendPlayerID(struct sockaddr_in si_other)
{
	unsigned short port = si_other.sin_port;
	Player* player = GetPlayerByPort(port, si_other);

	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);
	uint32_t mes = MessageType::PLAYER_NUMBER;

	memcpy(&buffer[0], &mes, sizeof(uint32_t));

	int playerNum = player->playerNumber;
	memcpy(&buffer[sizeof(uint32_t)], &playerNum, sizeof(int));

	int packetLength = sizeof(uint32_t) + sizeof(int);
	
	int result = sendto(mListenSocket, buffer, packetLength, 0,
			(struct sockaddr*)&(player->si_other), sizeof(player->si_other));
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// printf(".");		// Quick test
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}
	return;
}

