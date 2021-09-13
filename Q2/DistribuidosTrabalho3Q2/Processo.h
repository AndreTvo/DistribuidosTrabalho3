#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Network.h"
#include <thread>
using namespace std;

class Processo
{
private:
	int _processId;

	unordered_set<int> _multicastMembers;			// Key = ProcessID
	unordered_map<int, int> _multicastResponses;	// Key = ProcessID, Value = Response

	string _ip;
	unsigned short _port;
	bool _localRequest;
	bool _wrongAnswer;

	thread _multicastThread;
	void _MulticastListen();

	int _Funcao(int a);

	void _EnviarPedidoVotacao(UDPSocket& localSocket, UDPPacket& packet, int processId);
	void _VerificarRespostas(UDPSocket& localSocket, UDPPacket& packet);

public:
	Processo();
	~Processo();

	void iniciar();
};