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
#include "validation.h"

/*Serveur version 26 avril (djeber)
TODO : mettre les choses dans des fonctions
Tester les cas possibles afin de détécter des erreurs eventuelles */

int main(int argc, char **argv){
	int  socketConnexion,        /* descripteur socket connexion */
       joueur1,
       joueur2,       /* descripteur socket transmission */
       port = 0,            /* numero de port */
       sizeAddr,        /* taille de l'adresse d'une socket */
       err,	            /* code d'erreur */
       boucle1 = 0;
    struct sockaddr_in addClient;	/* adresse de la socket client connectee */   
    bool validation = true;    // utilisation de la validation
    bool timeout = true;       // utilisation de la limite deu temps
    bool fait1=false,fait2=false,firstIsFirst=true,firstIsSecond=false;
    char* time = "--noTimeout";
    char* valid = "--noValid";
    TPartieReq requetePartie1,requetePartie2;
    TPartieRep reponsePartie1,reponsePartie2;
    TSensTetePiece sensJoueur1,sensJoueur2;
    TCoupReq requeteCoupJoueur1,requeteCoupJoueur2;
    TCoupRep reponseCoupJoueur1,reponseCoupJoueur2;
    TPropCoup propCoupJoueur1,propCoupJoueur2;

    bool validationJoueur1=true, validationJoueur2=true, finPartie = false;
    fd_set  readSet;
    struct timeval tv;   // pour gérer le timeOut
	
 


    //verification des arguments 
       
    if (argc >= 2 || argc <= 4) {
    	if(argc == 2) port  = atoi(argv[1]);
  		
  		else if(argc == 3 && ( !(strcmp(argv[1],time)) || !(strcmp(argv[1],valid)) )) {
  			port  = atoi(argv[2]);

  			if (!(strcmp(argv[1],time))) timeout = false;  				
  			if (!(strcmp(argv[1],valid))) validation = false;
  	
  		}
  		else if(argc == 4 && (!(strcmp(argv[1],time)) || !(strcmp(argv[1],valid))) &&
  		 (!(strcmp(argv[2],time)) || !(strcmp(argv[2],valid))) && (strcmp(argv[1],argv[2]))){
  			port  = atoi(argv[3]);
  			if (!(strcmp(argv[1],time)) || !(strcmp(argv[2],time))) timeout = false;
  			if (!(strcmp(argv[1],valid)) || !(strcmp(argv[2],valid))) validation = false;
  				
  		}
  		else{
  			printf("usage : ./serveur [--noValid|--noTimeout] no_port\n");
  			return -1;
  		}
    
  	}
  	else{
  		printf("usage : ./serveur [--noValid|--noTimeout] no_port\n");
  		return -2;
  	}

  	if (timeout && validation ) printf("Starting ./serveur on port %d with validation and timeout \n",port);
  	else if (!timeout && validation) printf("Starting ./serveur on port %d with validation and  no timeout \n",port);
  	else if (timeout && !validation) printf("Starting ./serveur on port %d with no validation and timeout \n",port);
  	else if (!timeout && !validation) printf("Starting ./serveur on port %d with no validation and  no timeout \n",port);


  	//Création de la socket de connexion :

  	socketConnexion = socketServeur(port);

  	//Création des sockets de transmissions (une pour chaque joueur)

  	

	sizeAddr = sizeof(struct sockaddr_in);
	
	joueur1 = accept(socketConnexion,(struct sockaddr *)&addClient,(socklen_t *)&sizeAddr);
	if (joueur1 < 0) {
	   	perror("(serveur) erreur sur accept joueurSud");
		return -5;
  	}

  	joueur2 = accept(socketConnexion,(struct sockaddr *)&addClient,(socklen_t *)&sizeAddr);
	if (joueur2 < 0) {
    	perror("(serveur) erreur sur accept joueurSud");
    	return -5;
  	}

	FD_ZERO(&readSet);
	FD_SET(joueur1,&readSet);
	FD_SET(joueur2,&readSet);

	//Mettre le timeOut en place sur les sockets des joueurs
	/*
	if(timeout){
		tv.tv_sec = 6;
		tv.tv_usec = 0;
		setsockopt(joueur1, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
		setsockopt(joueur2, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	}*/

	//Utilisation du multiplexage
	err = select(FD_SETSIZE, &readSet, NULL, NULL, NULL);
    if (err < 0) {
      	perror("(serveurSelect) erreur dans select");
      	shutdown(socketConnexion, SHUT_RDWR); close(socketConnexion);
    return 5;
    }

    //Traitement de requetes Partie
    while(boucle1 < 2){
    	FD_ZERO(&readSet);
		FD_SET(joueur1,&readSet);
		FD_SET(joueur2,&readSet);
	    if (FD_ISSET(joueur1,&readSet)!=0 && !fait1)
	    {
	    	//reception pour joueur 1
	    	err = recv(joueur1, &requetePartie1, sizeof(TPartieReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur1, SHUT_RDWR); close(joueur1);
			return -4;
			}
			if (requetePartie1.piece == SUD) printf("Recu demande partie : nom %s \t\t/ sens SUD\n",requetePartie1.nomJoueur);
			if (requetePartie1.piece == NORD) printf("Recu demande partie : nom %s \t\t/ sens NORD\n",requetePartie1.nomJoueur);
			if (boucle1==0) {reponsePartie1.validSensTete = OK;}
			else{
				if (requetePartie1.piece == requetePartie2.piece)
				{
					reponsePartie1.validSensTete = KO;
				}
				else{
					reponsePartie1.validSensTete = OK;
				}
			} 
			strncpy(reponsePartie1.nomAdvers, "bidon2", T_NOM);
			reponsePartie1.err = 0;
			err = send(joueur1, &reponsePartie1, sizeof(TPartieRep), 0);
    		if (err <= 0){
      			perror("(client) erreur sur le send");
      			shutdown(joueur1, SHUT_RDWR); close(joueur1);
      		return -3;
    		}
	    	boucle1++;
	    	fait1 = true;
	    	
	    	
	    }
	    
	    if (FD_ISSET(joueur2,&readSet)!=0 && !fait2)
	    {
	    	err = recv(joueur2, &requetePartie2, sizeof(TPartieReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur2, SHUT_RDWR); close(joueur2);
			return -4;
			}
			if (requetePartie2.piece == SUD) printf("Recu demande partie : nom %s \t\t/ sens SUD\n",requetePartie2.nomJoueur);
			if (requetePartie2.piece == NORD) printf("Recu demande partie : nom %s \t\t/ sens NORD\n",requetePartie2.nomJoueur);
			if (boucle1==0) {reponsePartie2.validSensTete = OK;}
			else{
				if (requetePartie2.piece == requetePartie1.piece)
				{
					reponsePartie2.validSensTete = KO;
				}
				else{
					reponsePartie2.validSensTete = OK;
				}
			} 
			strncpy(reponsePartie2.nomAdvers, "bidon1", T_NOM);
			reponsePartie2.err = 0;
			err = send(joueur2, &reponsePartie2, sizeof(TPartieRep), 0);
    		if (err <= 0){
      			perror("(client) erreur sur le send");
      			shutdown(joueur2, SHUT_RDWR); close(joueur2);
      		return -3;
    		}
	    	boucle1++;
	    	fait2 = true;
	    }
	}
	printf("Pieces des joueurs validées : \n");

	//attribution des sens aux joueur
	if ((requetePartie1.piece == SUD && reponsePartie1.validSensTete == OK) || (requetePartie1.piece == NORD && reponsePartie1.validSensTete == KO)){
	 printf("joueur %s  -sens : SUD \n",requetePartie1.nomJoueur);
	 sensJoueur1 = SUD;
	 firstIsFirst = true;
	}

	if ((requetePartie1.piece == NORD && reponsePartie1.validSensTete == OK) || (requetePartie1.piece == SUD && reponsePartie1.validSensTete == KO)){
	 printf("joueur %s  -sens : NORD \n",requetePartie1.nomJoueur);
	 sensJoueur1 = NORD;
	 firstIsFirst = false;
	}
	if ((requetePartie2.piece == SUD && reponsePartie2.validSensTete == OK) || (requetePartie2.piece == NORD && reponsePartie2.validSensTete == KO)){
	 printf("joueur %s  -sens : SUD \n",requetePartie2.nomJoueur);
	 sensJoueur2 = SUD;
	 firstIsSecond = true;
	}

	if ((requetePartie2.piece == NORD && reponsePartie2.validSensTete == OK) || (requetePartie2.piece == SUD && reponsePartie2.validSensTete == KO)){
	 printf("joueur %s  -sens : NORD \n",requetePartie2.nomJoueur);
	 sensJoueur2 = NORD;
	 firstIsSecond = false;	
	}

	
	

	//dans le cas ou celui qui demande le sens en premier est SUS
	if (firstIsFirst)
	{
		//initialisation de la partie aller (1)
		initialiserPartie();
		printf("Début Partie Aller (Le joueur ayant sens SUD commence)\n");

		while(!finPartie){
			//reinitialiser les descripteurs
			FD_ZERO(&readSet);
			FD_SET(joueur1,&readSet);
			FD_SET(joueur2,&readSet);
			//recevoir la requete coup du premier joueur (SUD)
			err = recv(joueur1, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur1, SHUT_RDWR); close(joueur1);
			return -4;
			}
			//validation du coup du premier joueur (SUD)

			validationJoueur1 = validationCoup(1, requeteCoupJoueur1, &propCoupJoueur1);
			
			reponseCoupJoueur1.propCoup = propCoupJoueur1;
			if (validationJoueur1)
			{
				printf("recoitCoup : validation du coup du joueur SUD = VALID\n");
				reponseCoupJoueur1.err = ERR_OK;
				reponseCoupJoueur1.validCoup = VALID;
			}
			else{
				printf("recoitCoup : validation du coup du joueur SUD = TRICHE\n");
				reponseCoupJoueur1.err = ERR_OK;
				reponseCoupJoueur1.validCoup = TRICHE;
			}
			//envoi de la validation au joueur 1 (SUD)
			err = send(joueur1, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				
			}

			//envoi de la validation au joueur 2 (NORD)
			err = send(joueur2, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				
			}
			//La partie aller se continue
			if (validationJoueur1 && propCoupJoueur1==CONT)
			{
				//envoi du coup au joueur adverse (NORD)
				err = send(joueur2, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur2, SHUT_RDWR); close(joueur2);
					
				}
				//recevoir la requete coup du deuxième joueur (NORD)
				err = recv(joueur2, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0) {
					perror("(clientTCP) erreur dans la reception");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				return -4;
				}
				//validation du coup du deuxième joueur (NORD)

				validationJoueur2 = validationCoup(2, requeteCoupJoueur2, &propCoupJoueur2);
				if (validationJoueur2) printf("recoitCoup : validation du coup du joueur NORD = VALID\n");

				

				reponseCoupJoueur2.propCoup = propCoupJoueur2;
				if (validationJoueur2)
				{
					reponseCoupJoueur2.err = ERR_OK;
					reponseCoupJoueur2.validCoup = VALID;
				}
				else{
					reponseCoupJoueur2.err = ERR_OK;
					reponseCoupJoueur2.validCoup = TRICHE;
				}
				//envoi de la validation au joueur 2 (NORD)
				err = send(joueur2, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur2);
					
				}

				//envoi de la validation au joueur 1 (SUD)
				err = send(joueur1, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}
				//envoi du coup au joueur adverse (SUD)
				if (validationJoueur2 && propCoupJoueur2==CONT)
				{
					err = send(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
					if (err <= 0){
							perror("(joueur) erreur sur le send coup");
							shutdown(joueur1, SHUT_RDWR); close(joueur1);
						
					}
				}
				else{
					finPartie = true;
				}
			}
			else{
				finPartie = true;
			}

			
		}
		//pour commencer la partie retour
		finPartie = false;

		//Partie retour 

		//initialisation de la partie aller (1)
		//initialiserPartie();
		printf("Début Partie Retour (Le joueur ayant sens NORD commence)\n");

		while(!finPartie){
			//reinitialiser les descripteurs
			FD_ZERO(&readSet);
			FD_SET(joueur1,&readSet);
			FD_SET(joueur2,&readSet);
			//recevoir la requete coup du premier joueur (NORD)
			printf("Attente coup joueur (NORD)\n");
			err = recv(joueur2, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur2, SHUT_RDWR); close(joueur2);
			return -4;
			}
			//validation du coup du premier joueur (NORD)

			validationJoueur2 = validationCoup(1, requeteCoupJoueur2, &propCoupJoueur2);

			

			reponseCoupJoueur2.propCoup = propCoupJoueur2;
			if (validationJoueur2)
			{
				reponseCoupJoueur2.err = ERR_OK;
				reponseCoupJoueur2.validCoup = VALID;
			}
			else{
				reponseCoupJoueur2.err = ERR_OK;
				reponseCoupJoueur2.validCoup = TRICHE;
			}
			//envoi de la validation au joueur 1 (NORD)
			err = send(joueur2, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				
			}

			//envoi de la validation au joueur 2 (SUD)
			err = send(joueur1, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				
			}
			//la partie aller se continue
			if (validationJoueur2 && propCoupJoueur2==CONT)
			{
				//envoi du coup au joueur adverse (SUD)
				err = send(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}
				//recevoir la requete coup du deuxième joueur (SUD)
				err = recv(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0) {
					perror("(clientTCP) erreur dans la reception");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				return -4;
				}
				//validation du coup du deuxième joueur (SUD)

				validationJoueur1 = validationCoup(2, requeteCoupJoueur1, &propCoupJoueur1);

				

				reponseCoupJoueur1.propCoup = propCoupJoueur1;
				if (validationJoueur1)
				{
					reponseCoupJoueur1.err = ERR_OK;
					reponseCoupJoueur1.validCoup = VALID;
				}
				else{
					reponseCoupJoueur1.err = ERR_OK;
					reponseCoupJoueur1.validCoup = TRICHE;
				}
				//envoi de la validation au joueur 2 (SUD)
				err = send(joueur1, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}

				//envoi de la validation au joueur 2 (NORD)
				err = send(joueur2, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur2, SHUT_RDWR); close(joueur2);
					
				}
				//envoi du coup au joueur adverse (NORD)
				if (validationJoueur1 && propCoupJoueur1==CONT)
				{
					err = send(joueur2, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
					if (err <= 0){
							perror("(joueur) erreur sur le send coup");
							shutdown(joueur2, SHUT_RDWR); close(joueur2);
					}
				}
				else{
					finPartie = true;
				}
			}
			else{
				finPartie = true;
			}

			
		}


			
			
			
		
			


		}
		

	finPartie = false;

	//dans le cas ou celui qui demande le sens en deuxième est SUD
	if (firstIsSecond)
	{
		//initialisation de la partie aller (1)
		initialiserPartie();
		printf("Début Partie Aller (Le joueur ayant sens SUD commence)\n");

		while(!finPartie){
			//reinitialiser les descripteurs
			FD_ZERO(&readSet);
			FD_SET(joueur1,&readSet);
			FD_SET(joueur2,&readSet);
			//recevoir la requete coup du premier joueur (NORD)
			printf("Attente coup joueur (NORD)\n");
			err = recv(joueur2, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur2, SHUT_RDWR); close(joueur2);
			return -4;
			}
			//validation du coup du premier joueur (NORD)

			validationJoueur2 = validationCoup(1, requeteCoupJoueur2, &propCoupJoueur2);

			

			reponseCoupJoueur2.propCoup = propCoupJoueur2;
			if (validationJoueur2)
			{
				reponseCoupJoueur2.err = ERR_OK;
				reponseCoupJoueur2.validCoup = VALID;
			}
			else{
				reponseCoupJoueur2.err = ERR_OK;
				reponseCoupJoueur2.validCoup = TRICHE;
			}
			//envoi de la validation au joueur 1 (NORD)
			err = send(joueur2, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				
			}

			//envoi de la validation au joueur 2 (SUD)
			err = send(joueur1, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				
			}
			//la partie aller se continue
			if (validationJoueur2 && propCoupJoueur2==CONT)
			{
				//envoi du coup au joueur adverse (SUD)
				err = send(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}
				//recevoir la requete coup du deuxième joueur (SUD)
				err = recv(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0) {
					perror("(clientTCP) erreur dans la reception");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				return -4;
				}
				//validation du coup du deuxième joueur (SUD)

				validationJoueur1 = validationCoup(2, requeteCoupJoueur1, &propCoupJoueur1);

				

				reponseCoupJoueur1.propCoup = propCoupJoueur1;
				if (validationJoueur1)
				{
					reponseCoupJoueur1.err = ERR_OK;
					reponseCoupJoueur1.validCoup = VALID;
				}
				else{
					reponseCoupJoueur1.err = ERR_OK;
					reponseCoupJoueur1.validCoup = TRICHE;
				}
				//envoi de la validation au joueur 2 (SUD)
				err = send(joueur1, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}

				//envoi de la validation au joueur 2 (NORD)
				err = send(joueur2, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur2, SHUT_RDWR); close(joueur2);
					
				}
				//envoi du coup au joueur adverse (NORD)
				if (validationJoueur1 && propCoupJoueur1==CONT)
				{
					err = send(joueur2, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
					if (err <= 0){
							perror("(joueur) erreur sur le send coup");
							shutdown(joueur2, SHUT_RDWR); close(joueur2);
					}
				}
				else{
					finPartie = true;
				}
			}
			else{
				finPartie = true;
			}

			
		}
		//pour commencer la partie retour
		finPartie = false;

		//Partie retour 

		//initialisation de la partie aller (1)
		initialiserPartie();
		printf("Début Partie Retour (Le joueur ayant sens NORD commence)\n");

		while(!finPartie){
			//reinitialiser les descripteurs
			FD_ZERO(&readSet);
			FD_SET(joueur1,&readSet);
			FD_SET(joueur2,&readSet);
			//recevoir la requete coup du premier joueur (SUD)
			err = recv(joueur1, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
			if (err <= 0) {
				perror("(clientTCP) erreur dans la reception");
				shutdown(joueur1, SHUT_RDWR); close(joueur1);
			return -4;
			}
			//validation du coup du premier joueur (SUD)

			validationJoueur1 = validationCoup(1, requeteCoupJoueur1, &propCoupJoueur1);
			
			reponseCoupJoueur1.propCoup = propCoupJoueur1;
			if (validationJoueur1)
			{
				printf("recoitCoup : validation du coup du joueur SUD = VALID\n");
				reponseCoupJoueur1.err = ERR_OK;
				reponseCoupJoueur1.validCoup = VALID;
			}
			else{
				printf("recoitCoup : validation du coup du joueur SUD = TRICHE\n");
				reponseCoupJoueur1.err = ERR_OK;
				reponseCoupJoueur1.validCoup = TRICHE;
			}
			//envoi de la validation au joueur 1 (SUD)
			err = send(joueur1, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur1, SHUT_RDWR); close(joueur1);
				
			}

			//envoi de la validation au joueur 2 (NORD)
			err = send(joueur2, &reponseCoupJoueur1, sizeof(TCoupRep), 0);
			if (err <= 0){
					perror("(joueur) erreur sur le send coup");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				
			}
			//La partie aller se continue
			if (validationJoueur1 && propCoupJoueur1==CONT)
			{
				//envoi du coup au joueur adverse (NORD)
				err = send(joueur2, &requeteCoupJoueur1, sizeof(TCoupReq), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur2, SHUT_RDWR); close(joueur2);
					
				}
				//recevoir la requete coup du deuxième joueur (NORD)
				err = recv(joueur2, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
				if (err <= 0) {
					perror("(clientTCP) erreur dans la reception");
					shutdown(joueur2, SHUT_RDWR); close(joueur2);
				return -4;
				}
				//validation du coup du deuxième joueur (NORD)

				validationJoueur2 = validationCoup(2, requeteCoupJoueur2, &propCoupJoueur2);
				if (validationJoueur2) printf("recoitCoup : validation du coup du joueur NORD = VALID\n");

				

				reponseCoupJoueur2.propCoup = propCoupJoueur2;
				if (validationJoueur2)
				{
					reponseCoupJoueur2.err = ERR_OK;
					reponseCoupJoueur2.validCoup = VALID;
				}
				else{
					reponseCoupJoueur2.err = ERR_OK;
					reponseCoupJoueur2.validCoup = TRICHE;
				}
				//envoi de la validation au joueur 2 (NORD)
				err = send(joueur2, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur2);
					
				}

				//envoi de la validation au joueur 1 (SUD)
				err = send(joueur1, &reponseCoupJoueur2, sizeof(TCoupRep), 0);
				if (err <= 0){
						perror("(joueur) erreur sur le send coup");
						shutdown(joueur1, SHUT_RDWR); close(joueur1);
					
				}
				//envoi du coup au joueur adverse (SUD)
				if (validationJoueur2 && propCoupJoueur2==CONT)
				{
					err = send(joueur1, &requeteCoupJoueur2, sizeof(TCoupReq), 0);
					if (err <= 0){
							perror("(joueur) erreur sur le send coup");
							shutdown(joueur1, SHUT_RDWR); close(joueur1);
						
					}
				}
				else{
					finPartie = true;
				}
			}
			else{
				finPartie = true;
			}

			
		}

		


			
	}	




	return 0;
}