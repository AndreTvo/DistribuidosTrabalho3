#include "Processo.h"

#include <sstream>
#include <cstdlib>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

void Processo::_MulticastListen()
{
	stringstream ss;

	UDPSocket multicastSocket;
	multicastSocket.JoinMulticast(_ip);
	multicastSocket.Bind(_port);

	UDPPacket* packet;
	char buffer[sizeof(UDPPacket)] = { 0 };

	while (true)
	{
		sockaddr_in memberSocket = multicastSocket.RecvFrom(buffer, sizeof(UDPPacket));
		packet = reinterpret_cast<UDPPacket*>(buffer);

		switch (packet->messageType)
		{
			// Novo Membro
			// Adiciona a lista a si mesmo e se apresenta ao multicast
			case 0:
				// Não aceita pacote de si mesmo e não aceita membros ja existentes (evitar flood de pacotes inuteis)
				if (packet->processId == _processId || _multicastMembers.count(packet->processId))
					break;

				_multicastMembers.emplace(packet->processId);
				packet->processId = _processId;

				multicastSocket.SendTo(_ip, _port, buffer, sizeof(UDPPacket));
				break;

			// Remover Membro
			case 1:
				if (_multicastMembers.count(packet->processId))
				{
					_multicastMembers.erase(packet->data);
				}
				break;

			// Votacao
			case 2:
				if (_multicastMembers.count(packet->processId))
				{
					// Votar SIM na remoção
					packet->messageType = 6;
					packet->processId = _processId;
					packet->data = _wrongAnswer ? 1 : 0;

					multicastSocket.SendTo(_ip, _port, buffer, sizeof(UDPPacket));
				}
				break;

			// Funcao
			case 3:
				if (_multicastMembers.count(packet->processId))
				{
					packet->messageType = 5;
					packet->processId = _processId;
					packet->data = _wrongAnswer ? 100 : packet->data * 100;

					multicastSocket.SendTo(_ip, _port, buffer, sizeof(UDPPacket));
				}
				break;

			// Resposta Funcao
			case 5:
				if (_multicastMembers.count(packet->processId))
				{
					// Se o processo atual foi quem fez o pedido de votação, fazer a contagem
					// Se não, descarta pacote
					if (_localRequest)
					{
						int key = packet->processId;
						int value = packet->data;

						_multicastResponses[key] = value;
					}
				}
				break;

			// Resposta Votacao
			case 6:
				if (_multicastMembers.count(packet->processId))
				{
					// Se o processo atual foi quem fez o pedido de votação, fazer a contagem
					// Se não, descarta pacote
					if (_localRequest)
					{
						if (_multicastResponses.count(packet->data))
							_multicastResponses[packet->data] = _multicastResponses[packet->data] + 1;
						else
							_multicastResponses[packet->data] = 1;
					}
				}
				break;
		}
	}
}

int Processo::_Funcao(int a)
{
	return 0;
}

void Processo::_EnviarPedidoVotacao(UDPSocket& localSocket, UDPPacket& packet, int processId)
{
	// Enviar pedido de votação
	packet.messageType = 2;
	packet.processId = _processId;
	packet.data = processId;
	localSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&packet), sizeof(UDPPacket));

	cout << "Enviando votação para remover o processo de identificação " << processId << endl;

	this_thread::sleep_for(seconds(2));

	bool remover = _multicastResponses[0] > _multicastResponses[1];

	cout << "Verificando votação..." << endl;
	cout << "Votos SIM: " << _multicastResponses[0] << endl;
	cout << "Votos NAO: " << _multicastResponses[1] << endl;
	cout << "\n";

	if (remover)
	{
		// Enviar pacote informando a remoção do membro
		packet.messageType = 1;
		packet.processId = _processId;
		packet.data = processId;
		localSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&packet), sizeof(UDPPacket));
	}

	if (_multicastResponses.count(0))
		_multicastResponses.erase(0);

	if (_multicastResponses.count(1))
		_multicastResponses.erase(1);
}

void Processo::_VerificarRespostas(UDPSocket& localSocket, UDPPacket& packet)
{
	// Todos responderam
	// Verificar respostas
	// Key = Resposta
	// Value = Votos
	int respostaMenosVotos = INT_MAX;
	int respostaSelecionada = 0;
	unordered_map<int, int> respostas;

	// Conta quantas respostas repetidas
	for (auto it : _multicastResponses)
	{
		if (respostas.count(it.second))
		{
			respostas[it.second] = respostas[it.second] + 1;
		}
		else
		{
			// Primeiro voto
			respostas[it.second] = 1;
		}
	}

	for (auto it : respostas)
	{
		if (it.second < respostaMenosVotos)
		{
			respostaSelecionada = it.first;
			respostaMenosVotos = it.second;
		}
	}

	if (respostas.size() > 1)
	{
		// Alguem respondeu diferente
		// Verificar quem respondeu diferente
		// Key = Resposta
		// Value = Votos

		// Membros que votaram diferente do da maioria vão ser removidos
		unordered_set<int> selectedMembers;

		for (auto it : _multicastResponses)
		{
			if (it.second == respostaSelecionada)
				selectedMembers.emplace(it.first);
		}

		_multicastResponses.clear();
		_localRequest = true;

		for (auto it : selectedMembers)
		{
			_EnviarPedidoVotacao(localSocket, packet, it);
		}

		_localRequest = false;
	}
	else
	{
		cout << "Todos responderam o mesmo valor" << endl;
		// Todos responderam corretamente
		// OK
	}

	_multicastResponses.clear();
}

Processo::Processo()
{
	srand(time(NULL));

	_port = 1234;
	_ip = "224.1.50.15";
	_processId = rand();

	_localRequest = false;
	_wrongAnswer = false;
}

Processo::~Processo()
{
	_multicastThread.detach();
}

void Processo::iniciar()
{
	_multicastThread = thread(&Processo::_MulticastListen, this);
	
	UDPSocket localSocket;
	localSocket.Bind(0);
	
	int op = -1;

	cout << "Process ID: " << _processId << endl;

	UDPPacket packet;
	packet.messageType = 0;
	packet.processId = _processId;
	packet.data = 0;

	_multicastMembers.emplace(_processId);
	localSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&packet), sizeof(packet));

	while (op != 0)
	{
		cout << "\n\nDigite a função a ser executada: " << endl;
		cout << "1 - Funcao()" << endl;
		cout << "2 - Enviar resposta errada" << endl;
		cout << "3 - Enviar resposta correta" << endl;
		cout << "4 - Listar membros do multicast" << endl;
		cout << "0 - Sair" << endl;
		cin >> op;

		switch (op)
		{
			case 1:
			{
				_localRequest = true;

				packet.messageType = 3;
				packet.processId = _processId;
				packet.data = 10;
				localSocket.SendTo(_ip, _port, reinterpret_cast<char*>(&packet), sizeof(packet));

				cout << "Enviando pacote para Funcao()" << endl;
				cout << "Esperando 5 segundos por respostas..." << endl;

				this_thread::sleep_for(seconds(5));

				int numRespostas = 0;

				if (_multicastResponses.size() == _multicastMembers.size())
				{
					_VerificarRespostas(localSocket, packet);
				}
				else
				{
					// Processos que não responderam 
					// Havera um pedido de votacao para remove-los
					// Primeiro identifica quais não responderam e envia pedido de remoção pra cada um
					vector<int> processos;

					for (auto it : _multicastMembers)
					{
						if (!_multicastResponses.count(it))
						{
							processos.push_back(it);
						}
					}

					for (auto it : processos)
					{
						_EnviarPedidoVotacao(localSocket, packet, it);
					}

					_VerificarRespostas(localSocket, packet);
				}

				_localRequest = false;

				break;

			}

			case 2:
			{
				_wrongAnswer = true;
				break;
			}

			case 3:
			{
				_wrongAnswer = false;
				break;
			}

			case 4:
			{
				cout << "Membros do multicast registrados: " << endl;
				for (auto it : _multicastMembers)
				{
					cout << "ProcessID " << it << endl;
				}

				break;
			}
		}
	}

}
