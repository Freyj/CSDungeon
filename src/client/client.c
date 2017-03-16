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
	adresse_locale.sin_port = htons(port);

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

int getSourceLongueur(char* mesg){
	int nomSourceLongueur;
	nomSourceLongueur = 0;
	nomSourceLongueur = nomSourceLongueur + 10 * atoi(mesg[1]);
	nomSourceLongueur = nomSourceLongueur + atoi(mesg[2]);
	return nomSourceLongueur;
}

int getCibleLongueur(char* mesg){
	int nomCibleLongueur;
	nomCibleLongueur = 0;
	nomCibleLongueur = nomCibleLongueur + 10 * atoi(mesg[3]);
	nomCibleLongueur = nomCibleLongueur + atoi(mesg[4]);
	return nomCibleLongueur;
}

int getDonneesLongueur(char* mesg){
	int donneesLongueur;
	donneesLongueur = 0;
	donneesLongueur = donneesLongueur + 100 * atoi(mesg[6]);
	donneesLongueur = donneesLongueur + 10* atoi(mesg[7]);
	donneesLongueur = donneesLongueur + atoi(mesg[8]);
	return donneesLongueur;
}

int getPointsDeVie(char* mesg, int pos){
	int pointsDeVie;
	pointsDeVie = 0;
	pointsDeVie = pointsDeVie + 100 * atoi(mesg[pos + 1]);;
	pointsDeVie = pointsDeVie + 10 * atoi(mesg[pos + 2]);;
	pointsDeVie = pointsDeVie + atoi(mesg[pos + 3]);;
	return pointsDeVie;
}


void decode(char* mesg) {
	int nomSourceLongueur;
	int nomCibleLongueur;
	int donneesLongueur;
	int typeDeModification;
	int pointsDeVie;
	char* nomSource;
	char* nomCible;
	int longueurEntete;
	longueurEntete = 9;
	if(mesg[0] == "0"){									//0
		//connexion
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		//nomCibleLongueur inutile;						//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);

	}else if(mesg[0] == "1"){							//0
		//attaque
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
	}else if(mesg[0] == "2"){							//0
		//soigne
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
	}else if(mesg[0] == "3"){							//0
		//notification
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		typeDeModification = atoi(mesg[5]);				//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
	}else if(mesg[0] == "4"){							//0
		//mort
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);

	}else{
		perror("erreur : message errone.");
		exit(1);
	}
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
