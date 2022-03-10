#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <string>
#include "GLMCommon.h"

using std::string;

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

class UDPClient
{
public:
	UDPClient(void);
	~UDPClient(void);

	void CreateSocket(string ip, int port);
	void Update(void);

	void Send(char* data, int numBytes);
	void SendStart();
	void SendPosition();
	void SendNonPredictedPosition(glm::vec3 wantToMove);

	void SetPosition(int id, float& x, float& y);

	float UPDATES_PER_SEC = 5;
private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	SOCKET mServerSocket;
	struct sockaddr_in si_other;
};