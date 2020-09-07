#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "fonctionsTCP.h"
#include "protocolYokai.h"
#include "outils.h"

//// 10/05/2019


int main(int argc, char **argv){

	int sock,port,err,sockJava,portJava,coupsJoues = 0,sensJava;              
	char* nomMachServ;       
	TPartieReq partieReq;
	TPartieRep partieRep;
	TCoupReq coupReq,coupReqAdversaire;
	TSensTetePiece sensAccorde;
	CoupAdvJava coupAdvJava;
	bool breaker = false; 

		 
	/* verification des arguments */
	if (argc != 4) {
	   printf("usage : %s nom/IPServ port-arbitre port-moteur-java\n", argv[0]);
	   return -1;
	}
	//affectation des arguments  
	nomMachServ = argv[1];
	port = atoi(argv[2]);
	portJava = atoi(argv[3]);

	//etablir une connexion avec l'arbitre
	sock = socketClient(nomMachServ, port);
	if (sock < 0) {
	    perror("(client) erreur sur la creation de socket");
	    return -2;
	}

	//etablir une connexion avec le moteur java
	sockJava = socketClient(nomMachServ, portJava);
	if (sockJava < 0) {
	    perror("(client) erreur sur la creation de socket");
	    return -2;
	}

	//Envoi de la requete Partie
	sensAccorde = debutPartie(sock, partieReq, partieRep);

	//indiquer le sens au moteur java
	if (sensAccorde==SUD) sensJava = -1;
	else sensJava = 1;

	//envoi du sens au moteur Java
	sensJava = htonl(sensJava);
    err = send(sockJava, &sensJava, sizeof(int), 0);
	if (err <= 0){
		perror("(joueur) erreur sur le send coup");
		shutdown(sockJava, SHUT_RDWR); close(sockJava);
	
	}
	printf("Sens accordé indiqué à Java \n");
	//déroulement des parties
	switch (sensAccorde){
		//on commence en premier
		case SUD:
			printf("\n\n");
			printf("******************************* \n");
			printf("********** Partie aller ******* \n");
			printf("******************************* \n\n\n");
			
			printf("** C'est a vous de commencer ** \n");
			
			while(coupsJoues <= NB_COUP_MAX){
				
				//Construction d'un coup (Java)
				printf("Construction du coup en cours \n");
			    coupReq = construireCoup(sockJava,SUD,1,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

			    //envoi du coup au serveur
			    err = send(sock, &coupReq, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(sock, SHUT_RDWR); close(sock);
					break;
				}
    			printf("Coup construit et envoyé \n");
    			coupsJoues++;
    			
    			//reception de la validation 
			    receptionValidation(sock,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }	
				
				//reception validation adverse + coup adverse
    			coupReqAdversaire = receptionAdverse(sock,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

				//Construction d'un Move pour Jasper
    			coupAdvJava = construireMove(coupReqAdversaire,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
				err = send(sockJava, &coupAdvJava, sizeof(CoupAdvJava), 0);
				if (err <= 0){
					perror("(joueur) erreur sur le send coup adversaire Java");
					shutdown(sockJava, SHUT_RDWR); close(sockJava);
				break;
				}
				printf("Coup adversaire envoyé à Java\n");
			}


			printf("******************************* \n");
			printf("*** Vous jouez en deuxième **** \n");
			printf("******************************* \n");
			//////**************************************

			printf("\n\n");
			printf("******************************* \n");
			printf("************ Partie retour ********* \n");
			printf("******************************* \n\n");

			//reinitialiser les variables
			breaker = false;
			coupsJoues = 0;
			while(coupsJoues <= NB_COUP_MAX){
				//reception validation adverse + coup adverse
    			coupReqAdversaire = receptionAdverse(sock,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

				//Construction d'un Move pour Jasper
    			coupAdvJava = construireMove(coupReqAdversaire,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
				err = send(sockJava, &coupAdvJava, sizeof(CoupAdvJava), 0);
				if (err <= 0){
					perror("(joueur) erreur sur le send coup adversaire Java");
					shutdown(sockJava, SHUT_RDWR); close(sockJava);
				break;
				}
				printf("Coup adversaire envoyé à Java\n");
				//Construction d'un coup
				printf("Construction du coup en cours \n");
				coupReq = construireCoup(sockJava,SUD,2,&breaker);
				if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

				//envoyer le coup au serveur
				err = send(sock, &coupReq, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(sock, SHUT_RDWR); close(sock);
					break;
				}
				printf("Coup construit et envoyé\n");
    			//reception de la validation 
			    receptionValidation(sock,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
			}

		break;

		//reinitialiser les variables
		breaker = false;
		coupsJoues = 0;

		case NORD:

			printf("\n\n");
			printf("******************************* \n");
			printf("********** Partie aller ******* \n");
			printf("******************************* \n\n\n");

			printf("******************************* \n");
			printf("*** Vous jouez en deuxième **** \n");
			printf("******************************* \n");
			//////**************************************
			while(coupsJoues <= NB_COUP_MAX){
				//reception validation adverse + coup adverse
    			coupReqAdversaire = receptionAdverse(sock,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }	

    			//Construction d'un Move pour Jasper
    			coupAdvJava = construireMove(coupReqAdversaire,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
    			
				
    			//envoi du move au Jasper
				err = send(sockJava, &coupAdvJava, sizeof(CoupAdvJava), 0);
				if (err <= 0){
					perror("(joueur) erreur sur le send coup adversaire Java");
					shutdown(sockJava, SHUT_RDWR); close(sockJava);
				break;
				}
				printf("Coup adversaire envoyé à Java\n");

				//Construction d'un coup
				printf("Construction du coup en cours... \n");
				coupReq = construireCoup(sockJava,NORD,1,&breaker);
				if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
				err = send(sock, &coupReq, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(sock, SHUT_RDWR); close(sock);
					break;
				}
				printf("Coup construit et envoyé\n");
				coupsJoues++;

    			//reception de la validation 
			    receptionValidation(sock,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }	
			}

			

			printf("\n\n");
			printf("******************************* \n");
			printf("************ Partie retour ********* \n");
			printf("******************************* \n\n");
			printf("** C'est a vous de commencer ** \n");

			//reinitialiser les variables
			breaker = false;
			coupsJoues = 0;

			while(coupsJoues <= NB_COUP_MAX){
				//Construction d'un coup (Java)
				printf("Construction du coup en cours \n");
			    coupReq = construireCoup(sockJava,NORD,2,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
			    //envoi du coup
			    err = send(sock, &coupReq, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(sock, SHUT_RDWR); close(sock);
					break;
				}
    			printf("Coup construit et envoyé \n");
    			coupsJoues++;

			    //reception de la validation 
			    receptionValidation(sock,&breaker);
			    if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

				//reception validation adverse + coup adverse
    			coupReqAdversaire = receptionAdverse(sock,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }

				//Construction d'un Move pour Jasper
    			coupAdvJava = construireMove(coupReqAdversaire,&breaker);
    			if(breaker){
			    	int stop = 666;
			    	stop = ntohl(stop);
			    	err = send(sockJava, &stop, sizeof(int), 0);
			    	breaker = false;
					if (err <= 0){
						perror("(joueur) erreur sur le send coup adversaire Java");
						shutdown(sockJava, SHUT_RDWR); close(sockJava);
					break;
					}

			    	break;
			    }
				err = send(sockJava, &coupAdvJava, sizeof(CoupAdvJava), 0);
				if (err <= 0){
					perror("(joueur) erreur sur le send coup adversaire Java");
					shutdown(sockJava, SHUT_RDWR); close(sockJava);
				break;
				}
				printf("Coup adversaire envoyé à Java\n");	
			}

		break;
	}
	

	printf("Fermeture des sockets (c et java)\n");
	shutdown(sockJava, SHUT_RDWR); 
	close(sockJava);
	shutdown(sock, SHUT_RDWR);
	close(sock); 

  return 0;
}



  

  
