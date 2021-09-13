#include "Processo.h"
#include "Network.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

int Processo::_idRef = 0;

void Processo::resourceAccesses()
{
	// Simulação de acesso a area critica

	this_thread::sleep_for(milliseconds(1000));
	cout << ".";
	this_thread::sleep_for(milliseconds(750));
	cout << ".";
	this_thread::sleep_for(milliseconds(250));
	cout << ".";
	this_thread::sleep_for(milliseconds(2000));
	cout << ".";
	this_thread::sleep_for(milliseconds(250));
	cout << ".";
	this_thread::sleep_for(milliseconds(250));
	cout << ".";
	this_thread::sleep_for(milliseconds(250));
	cout << ".";
	this_thread::sleep_for(milliseconds(250));
	cout << ".";
	this_thread::sleep_for(milliseconds(2000));
	cout << "." << endl;
}

Processo::Processo()
{
	_id = _idRef;
	_idRef++;

	_permission = false;
	_resourceOwned = -1;
}

Processo::~Processo()
{
}

void Processo::configurar(string ip, short port)
{
	_ip = ip;
	_port = port;
}

void Processo::iniciar()
{
	cout << "Cliente iniciado: " << endl;

	UDPSocket udpSocket;
	UDPPacket udpPacket;

	int op = 0;
	int recurso = 0;

	while (true)
	{
		cout << "Digite a função a ser executada: " << endl;
		cout << "1 - enter()" << endl;
		cout << "2 - resourceAccesses()" << endl;
		cout << "3 - exit()" << endl;

		cin >> op;

		switch (op)
		{
			// enter()
			case 1:
				if (_resourceOwned < 0)
				{
					cout << "enter(): " << endl;
					cout << "Digite o ID do recurso a ser utilizado: ";
					cin >> recurso;

					udpPacket.messageType = 1;
					udpPacket.processId = _id;
					udpPacket.resourceId = recurso;

					udpSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&udpPacket), sizeof(UDPPacket));
					udpSocket.RecvFrom(reinterpret_cast<char*>(&udpPacket), sizeof(UDPPacket));

					if (udpPacket.messageType == 10)
					{
						_resourceOwned = recurso;

						cout << "Permissão para a area critica concedida" << endl;
						_permission = true;
					}
					else
					{
						cout << "Permissão para a area critica negada" << endl;
						_permission = false;
					}
				}
				break;

			// resourceAccesses()
			case 2:
				if (_permission)
				{
					cout << "resourceAccesses(): ";
					resourceAccesses();
				}
				else
				{
					cout << "O processo não tem permissão para acessar a area critica" << endl;
				}
				break;

			// exit()
			case 3:
				cout << "exit(): " << endl;
				udpPacket.messageType = 2;
				udpPacket.processId = _id;
				udpPacket.resourceId = _resourceOwned;

				udpSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&udpPacket), sizeof(UDPPacket));

				_permission = false;
				_resourceOwned = -1;
				break;
		}
	}
}
