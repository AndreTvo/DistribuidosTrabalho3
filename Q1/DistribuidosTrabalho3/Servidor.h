#pragma once

#include <unordered_map>
using namespace std;

class Servidor
{
private:
	// validResourceId = Key
	unordered_map<unsigned int, int> _validResources; 

	// resourceId = Key, processId = Value
	unordered_map<unsigned int, int> _resourcesTable;

	bool _enter(unsigned int resourceId, int processId);
	void _exit(unsigned int resourceId, int processId);

public:
	Servidor();
	~Servidor();

	void iniciar(string ip, short port);
};