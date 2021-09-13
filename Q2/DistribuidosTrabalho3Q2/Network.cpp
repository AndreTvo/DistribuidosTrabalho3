#include "Network.h"

WSASession::WSASession()
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
}

WSASession::~WSASession()
{
	WSACleanup();
}

UDPSocket::UDPSocket()
{
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}

UDPSocket::~UDPSocket()
{
	closesocket(sock);
}

void UDPSocket::JoinMulticast(string groupIp)
{
	u_int yes = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)& yes, sizeof(yes));
	if (ret < 0)
	{
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error setting multicast");
	}

	struct ip_mreq mreq;
	inet_pton(AF_INET, groupIp.c_str(), &mreq.imr_multiaddr.s_addr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (ret < 0)
	{
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error setting multicast");
	}
}

void UDPSocket::SendTo(const std::string & address, unsigned short port, const char* buffer, int len, int flags)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	inet_pton(AF_INET, address.c_str(), &add.sin_addr.s_addr);
	add.sin_port = htons(port);
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}

void UDPSocket::SendTo(sockaddr_in & address, const char* buffer, int len, int flags)
{
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}

sockaddr_in UDPSocket::RecvFrom(char* buffer, int len, int flags)
{
	sockaddr_in from;
	int size = sizeof(from);
	int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
	{
		int err = WSAGetLastError();

		if (err != 10040)
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
	}

	return from;
}

sockaddr_in UDPSocket::RecvFrom(char* buffer, int len, int& bytes, int flags)
{
	sockaddr_in from;
	int size = sizeof(from);
	int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
	{
		bytes = WSAGetLastError();
		int err = WSAGetLastError();
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
	}

	bytes = ret;

	return from;
}

void UDPSocket::Bind(const std::string & address, unsigned short port)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	inet_pton(AF_INET, address.c_str(), &add.sin_addr.s_addr);
	add.sin_port = htons(port);

	int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
}

void UDPSocket::Bind(unsigned short port)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_port = htons(port);
	add.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
}

SOCKET& UDPSocket::GetSOCKET()
{
	return sock;
}