/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.1
 * @brief : 
*/

/*-----------------------------------------------------------
  Client a lancer apres le serveur avec la commande :
  client <adresse-serveur> <message-a-transmettre>
  ------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct sockaddr 
sockaddr;

typedef struct sockaddr_in 
sockaddr_in;

typedef struct hostent 
hostent;

typedef struct servent 
servent;

/* Structure pour les armes */
typedef struct arme {
	char* nom;
	int estMagique;
	int degats;
	int precision;
}arme;

/* Structure pour stocker les infos du client */
typedef struct infoclient {
	/* nom  */
	char* nom;
	/* caractéristiques */
	int pv;
	int pvMax;
	int exp;
	int niveau;
	int force;
	int magie;
	int technique;
	int vitesse;
	int chance;
	int defense;
	int resistance;
	arme arme;
	/*socket associé */
}infoclient;


void sendMessage(int port, char* host, char* mesg) {
	int socket_descriptor; 		/* descripteur de socket */
	int longueur; 				/* longueur d'un buffer utilisé */

	sockaddr_in adresse_locale; /* adresse de socket local */

	hostent* ptr_host;  		/* info sur une machine hote */
	servent* ptr_service;  		/* info sur service */

	char buffer[256];
	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}
	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

	//adresse_locale.sin_port = htons(7332); // why port 7332 ?
	adresse_locale.sin_port = htons(5000);

	printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
	}

	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
	}
	if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}
	printf("message envoye au serveur. \n");
	//write(1,buffer,longueur);
	//return (longueur = read(socket_descriptor, buffer, sizeof(buffer)));
	while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		write(1,buffer,longueur);
    }
    printf("\n");
    close(socket_descriptor);

}


int main(int argc, char **argv) {
	int socket_descriptor; 		/* descripteur de socket */
//	int longueur; 				/* longueur d'un buffer utilisé */

//	sockaddr_in adresse_locale; /* adresse de socket local */

//	hostent* ptr_host;  		/* info sur une machine hote */
//	servent* ptr_service;  		/* info sur service */
//	char buffer[256];
	char* prog; 				/* nom du programme */
	char* host; 				/* nom de la machine distante */
	char* mesg = malloc(256); 	/* message envoyé */
	char* nomClient;
	int numeroPort;
	if (argc != 4) {
		perror("usage : client <adresse-serveur> <numero-port> <client-name>");
		exit(1);
	}
	
	prog = argv[0];
	host = argv[1];
	numeroPort = atoi(argv[2]);
	nomClient = argv[3];

	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);
	sendMessage(numeroPort, host, nomClient);
	int endConnection;
	endConnection = 1;
	while(endConnection > 0) {
		printf("Message : ");
		scanf("%s", mesg);
		printf("%s \n", mesg);

		sendMessage(numeroPort, host, mesg);
			if (strcmp("quit",mesg) == 0) {
				endConnection = 0;
			} else {
				endConnection = 1;
			}
	}
	free(mesg);
	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	exit(0);  
}
