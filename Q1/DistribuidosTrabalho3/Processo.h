#pragma once

#include <string>
#include <thread>
using namespace std;

class Processo
{
private:
	static int _idRef;
	int _id;
	int _resourceOwned;

	string _ip;
	short _port;

	bool _permission;

	void resourceAccesses();

public:
	Processo();
	~Processo();

	void configurar(string ip, short port);
	void iniciar();

};