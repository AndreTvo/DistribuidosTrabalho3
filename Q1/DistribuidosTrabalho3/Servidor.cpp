#include "Servidor.h"
#include "Network.h"

Servidor::Servidor()
{
	_validResources[0] = 0;
	_validResources[1] = 0;
	_validResources[2] = 0;
	_validResources[3] = 0;
	_validResources[4] = 0;
	_validResources[5] = 0;
	_validResources[6] = 0;
	_validResources[7] = 0;
	_validResources[8] = 0;
	_validResources[9] = 0;
}

Servidor::~Servidor()
{
}

void Servidor::iniciar(string ip, short port)
{
	cout << "Servidor iniciado: " << endl;

	UDPSocket udpSocket;
	udpSocket.Bind(ip, port);

	UDPPacket* udpPacket = nullptr;
	const int packetSize = sizeof(UDPPacket);
	char buffer[packetSize] = { 0 };

	bool access = false;

	while (true)
	{
		sockaddr_in clientSocket = udpSocket.RecvFrom(buffer, packetSize);
		udpPacket = reinterpret_cast<UDPPacket*>(buffer);

		switch (udpPacket->messageType)
		{
			case 1:
				access = _enter(udpPacket->resourceId, udpPacket->processId);

				udpPacket->messageType = access ? 10 : 20;
				udpPacket->processId = -1;
				udpSocket.SendTo(clientSocket, buffer, packetSize);
				break;

			case 2:
				_exit(udpPacket->resourceId, udpPacket->processId);
				break;
		}
	}
}

bool Servidor::_enter(unsigned int resourceId, int processId)
{
	if (!_validResources.count(resourceId))
		return false;

	if (_resourcesTable.count(resourceId))
	{
		// Recurso ocupado por outro processo
		return false;
	}

	_resourcesTable[resourceId] = processId;
	return true;
}

void Servidor::_exit(unsigned int resourceId, int processId)
{
	if (!_validResources.count(resourceId))
		return;

	if (_resourcesTable.count(resourceId))
	{
		// Recurso está sendo ocupado

		if (_resourcesTable[resourceId] == processId)
		{
			// Recurso está livre
			_resourcesTable.erase(resourceId);
		}

		// Recurso não está sendo ocupado pelo processo que fez o pedido de saida da seção critica
	}

	// Recurso não está sendo ocupado
}
