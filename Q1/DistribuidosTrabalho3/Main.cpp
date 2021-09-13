#include "Servidor.h"
#include "Processo.h"
#include "Network.h"

#include <locale.h> 
#include <iostream>
using namespace std;

int main()
{
	setlocale(LC_ALL, "Portuguese");

	WSASession wsaSession;

	string ip;
	short port;

	int op = 1;

	cout << "Digite o tipo de processo: " << endl;
	cout << "1 - Servidor" << endl;
	cout << "2 - Cliente" << endl;
	cin >> op;
	
	cout << "\nDigite o ip do servidor: ";
	cin >> ip;
	cout << "Digite a porta do servidor: ";
	cin >> port;
	
	if (op == 1)
	{
		Servidor servidor;
		servidor.iniciar(ip, port);
	}
	else
	{
		Processo processo;
		processo.configurar(ip, port);
		processo.iniciar();
	}


	return 0;
}