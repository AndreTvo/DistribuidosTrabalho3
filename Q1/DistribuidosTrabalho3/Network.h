#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>

using namespace std;

struct UDPPacket
{
	// Cliente:
	// 1 = Enter
	// 2 = Exit
	// Servidor
	// 10 = Acesso concedido
	// 20 = Acesso negado
	int messageType;
	int resourceId;

	// 0+ Cliente
	// -1 Servidor
	int processId;
};

class WSASession
{
public:
	WSASession();
	~WSASession();
private:
	WSAData data;
};

class UDPSocket
{
public:
	UDPSocket();
	~UDPSocket();

	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0);
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0);

	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0);
	sockaddr_in RecvFrom(char* buffer, int len, int& bytes, int flags);

	void Bind(const std::string& address, unsigned short port);
	void Bind(unsigned short port);

	SOCKET& GetSOCKET();

private:
	SOCKET sock;
};