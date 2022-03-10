#include "udp_client.h"

#include <bitset>
#include <iostream>
#include "globalThings.h"

enum MessageType
{
	START = 1,
	DONT_START = 2,
	UPDATE_POSITIONS = 3,
	GET_PLAYER_NUMBER = 4,
	NON_PREDICTED_POSITION = 5,
};

struct Player {
	float x, y, z;
	bool ready;
};

std::vector<Player> mPlayers;

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

void UDPClient::SetPosition(int id, float& x, float& y)
{
	x = mPlayers[id].x;
	y = mPlayers[id].y;
}

UDPClient::UDPClient(void)
	: mServerSocket(INVALID_SOCKET)
{
	mPlayers.resize(4);

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
	
}

UDPClient::~UDPClient(void)
{
	closesocket(mServerSocket);
	WSACleanup();
}

void UDPClient::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void UDPClient::CreateSocket(string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == SOCKET_ERROR) {
		PrintWSAError();
		double timer = 10.0;
		double deltaTime;
		double previousTime = std::clock();
		while (mServerSocket == SOCKET_ERROR)
		{
			double currentTime = std::clock();
			deltaTime = currentTime - previousTime;
			previousTime = currentTime;
			timer -= deltaTime;
			if (timer <= 0.0)
			{
				mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				timer = 10.0;
			}

		}
		return;
	}

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.s_addr = inet_addr(ip.c_str());

	SetNonBlocking(mServerSocket);
}

void UDPClient::Update(void)
{
	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	if (::playTime && ::clientSidePredictionOn)
	{
		SendPosition();
	}
	Recv();
}

void UDPClient::Recv(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mServerSocket, buffer, 512, 0, (struct sockaddr*)&si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}

	uint32_t messageID = buffer[0];
	//printf("MessageID: %d\n", messageID);
	switch (messageID)
	{
	case MessageType::START:
	{
		::playTime = true;
		//printf("Message Start\n");
	}
	break;
	case MessageType::DONT_START:
	{
		::playTime = false;
		//printf("Don't Start\n");
	}
	break;
	case MessageType::UPDATE_POSITIONS:
	{
		//printf("Update Positions\n");
		float x, y, z;
		int playerNum;
		for (unsigned int index = 0; index < 5; index++)
		{
			/*x = buffer[index * 16 + 4];
			y = buffer[index * 16 + 8];
			z = buffer[index * 16 + 12];
			playerNum = buffer[index * 16 + 16];*/
			memcpy(&x, &(buffer[index * 16 + 4]), sizeof(float));
			memcpy(&y, &(buffer[index * 16 + 8]), sizeof(float));
			memcpy(&z, &(buffer[index * 16 + 12]), sizeof(float));
			memcpy(&playerNum, &(buffer[index * 16 + 16]), sizeof(int));
		//	printf("Received Position: { %f %f %f } and playerNum: %d\n", x, y, z, playerNum);
			if (::clientSidePredictionOn)
			{
				if (playerNum != ::playerNumber)
				{
					::g_vec_pMeshes[index]->positionXYZ = glm::vec3(x, y, z);
				}
			}
			else
			{
				::g_vec_pMeshes[index]->positionXYZ = glm::vec3(x, y, z);
			}

			if (::deadReckoningOn)
			{
				// if deadReckoning is on then I want to get the velocity of the ball to use to predict it's position
				float velX, velY, velZ;
				memcpy(&velX, &(buffer[::g_vec_pMeshes.size() * 16 + 4]), sizeof(float));
				memcpy(&velY, &(buffer[::g_vec_pMeshes.size() * 16 + 8]), sizeof(float));
				memcpy(&velZ, &(buffer[::g_vec_pMeshes.size() * 16 + 12]), sizeof(float));
				::ballVelocity = glm::vec3(velX, velY, velZ);
			}

		}
	}
	break;
	case MessageType::GET_PLAYER_NUMBER:
	{
		int playerNum = buffer[sizeof(uint32_t)];
		::playerNumber = playerNum;
		printf("This is our player number: %d\n", ::playerNumber);
	}
	break;
	default:
	{
		printf("Not an appropriate message (probably don't print this)\n");
	}
	}

	// NumPlayers
	// Each player: { x, y }
	//unsigned int numPlayers;
	//memcpy(&numPlayers, &(buffer[0]), sizeof(unsigned int));

	//float x, y;
	//for (int i = 0; i < numPlayers; i++) {
	//	memcpy(&x, &(buffer[i * 8 + 4]), sizeof(float));
	//	memcpy(&y, &(buffer[i * 8 + 8]), sizeof(float));
	//	mPlayers[i].x = x;
	//	mPlayers[i].y = y;
	//}

	////unsigned short port = si_other.sin_port;
	////printf("%d : %hu received %d bytes\n", mServerSocket, port, result);

	//printf("%d players: {", numPlayers);
	//for (int i = 0; i < numPlayers; i++) {
	//	printf(" {x: %.2f, y: %.2f}", mPlayers[i].x, mPlayers[i].y);
	//}
	//printf(" }\n");
}

void UDPClient::Send(char* data, int numBytes)
{
	int result = sendto(mServerSocket, data, numBytes, 0,
		(struct sockaddr*)&si_other, sizeof(si_other));

	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		PrintWSAError();
		return;
	}

	if (result == 0) {
		printf("Disconnected...\n");
		return;
	}

	// printf("Number of bytes sent: %d\n", result);
}

void UDPClient::SendStart()
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	int mes = MessageType::START;
	memcpy(&(buffer[0]), &mes, sizeof(int));

	int result = sendto(mServerSocket, buffer, sizeof(int), 0, (struct sockaddr*)&si_other, sizeof(si_other));
}

void UDPClient::SendPosition()
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	int mes = MessageType::UPDATE_POSITIONS;
	memcpy(&(buffer[0]), &mes, sizeof(int));

	float x = ::g_vec_pMeshes[::playerNumber]->positionXYZ.x;
	float y = ::g_vec_pMeshes[::playerNumber]->positionXYZ.y;
	float z = ::g_vec_pMeshes[::playerNumber]->positionXYZ.z;

	memcpy(&(buffer[4]), &x, sizeof(float));
	memcpy(&(buffer[8]), &y, sizeof(float));
	memcpy(&(buffer[12]), &z, sizeof(float));

	int packetLength = sizeof(int) + (3 * sizeof(float));

	int result = sendto(mServerSocket, buffer, packetLength, 0, (struct sockaddr*)&si_other, sizeof(si_other));
}

void UDPClient::SendNonPredictedPosition(glm::vec3 wantToMove)
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	int mes = MessageType::NON_PREDICTED_POSITION;
	memcpy(&(buffer[0]), &mes, sizeof(int));

	float x = ::g_vec_pMeshes[::playerNumber]->positionXYZ.x + wantToMove.x;
	float y = ::g_vec_pMeshes[::playerNumber]->positionXYZ.y + wantToMove.y;
	float z = ::g_vec_pMeshes[::playerNumber]->positionXYZ.z + wantToMove.z;

	memcpy(&(buffer[4]), &x, sizeof(float));
	memcpy(&(buffer[8]), &y, sizeof(float));
	memcpy(&(buffer[12]), &z, sizeof(float));

	int packetLength = sizeof(int) + (3 * sizeof(float));

	int result = sendto(mServerSocket, buffer, packetLength, 0, (struct sockaddr*)&si_other, sizeof(si_other));
}