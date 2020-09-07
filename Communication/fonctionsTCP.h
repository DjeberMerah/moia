#include <stdbool.h>
#define _FCTTCP_

struct sockaddr_in trans;	
struct sockaddr_in conn;	
struct sockaddr_in nom;  
int sizeAddr;

int socketServeur(ushort nbPort);
int socketClient(char* nomMachine, ushort nbPort);


struct operation
{
	char operateur;
	int operande_un;
	int operande_deux;
	bool dernier;
	
	
};


