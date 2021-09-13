#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>

using namespace std;

struct UDPPacket
{
	// 0 = Novo Membro do Multicast
	// 1 = Remover Membro do Multicast
	// 2 = Votacao
	// 3 = Funcao
	// 5 = Resposta Funcao
	// 6 = Resposta Votacao
	int messageType;

	int processId;

	// Resposta da Funcao
	// 0 = Votacao Sim
	// 1 = Votacao Nao
	int32_t data;
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

	void JoinMulticast(string groupIp);

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