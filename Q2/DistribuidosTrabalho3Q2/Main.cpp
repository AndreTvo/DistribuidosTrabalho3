#include <iostream>
#include "Processo.h"
#include <locale.h> 

using namespace std;

int main()
{
	setlocale(LC_ALL, "Portuguese");

	WSASession wsaSession;

	Processo processo;
	processo.iniciar();

	return 0;
}