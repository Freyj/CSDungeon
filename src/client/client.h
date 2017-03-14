/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.1
 * @brief : 
*/

/*-----------------------------------------------------------
  Client a lancer apres le serveur avec la commande :
  client <adresse-serveur> <port> <pseudo>
  ensuite les commandes sont du type : 
  [attaquer|soigner] cible [option]
  le serveur gèrera l'ordre
  ------------------------------------------------------------*/
#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

/* Structure de définition pour les sockets */
typedef struct sockaddr 
sockaddr;

typedef struct sockaddr_in 
sockaddr_in;

typedef struct hostent 
hostent;

typedef struct servent 
servent;

/* Structure pour les armes 
typedef struct arme {
	char* nom;
	int estMagique;
	int degats;
	int precision;
}arme;
*/
/* Structure pour stocker les infos du client */
typedef struct infoclient {
	/* nom  */
	char* nom;
	/* caractéristiques */
	int pv;
	int pvMax;
	int exp;
	//int niveau;
	int force;
	//int magie;
	//int technique;
	//int vitesse;
	//int chance;
	//int defense;
	//int resistance;
	//arme arme;
	/*socket associé */
	int sock;
}infoclient;


#endif