
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

//#3E6EB7

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

int digit_to_int(char d){
	char str[2];
	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}

int getDonnees(char* mesg, int nbData, int data1Pos, int data2Pos, int data3Pos){
	int dataToReturn;
	dataToReturn = 0;
	if(nbData == 2){
		dataToReturn = dataToReturn + 10 * digit_to_int(mesg[data1Pos]);
		dataToReturn = dataToReturn + digit_to_int(mesg[data2Pos]);
	}else{
		dataToReturn = dataToReturn + 100 * digit_to_int(mesg[data1Pos]);
		dataToReturn = dataToReturn + 10 * digit_to_int(mesg[data2Pos]);
		dataToReturn = dataToReturn + digit_to_int(mesg[data3Pos]);
	}
	return dataToReturn;
}

int getSourceLongueur(char* mesg){
	return getDonnees(mesg, 2, 1, 2, 0);
}

int getCibleLongueur(char* mesg){
	return getDonnees(mesg, 2, 3, 4, 0);
}

int getDonneesLongueur(char* mesg){
	return getDonnees(mesg, 3, 6, 7, 8);
}

int getPointsDeVie(char* mesg, int offset){
	return getDonnees(mesg, 3, offset, offset + 1, offset + 2);
}

int getNbClient(char* mesg){
	return getDonnees(mesg, 2, 1, 2, 0);
}

int getLongueurNomClient(char* mesg, int offset){
	return getDonnees(mesg, 2, offset, offset + 1, 0);
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
	printf("\t\t");
	printf(mesg);
	printf("\n");
	if(mesg[0] == '0'){									//0
		//connexion
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		//nomCibleLongueur inutile;						//3 | 4
		//typeDeModification inutile					//5
		//donneesLongueur inutile						//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		printf("Type de message : CONNEXION \nLongueurNomSource : %i \nNomSource : %s\n",
		 nomSourceLongueur, nomSource);
		free(nomSource);
	}else if(mesg[0] == '1'){							//0
		//attaque
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("Type de message : ATTAQUE \nLongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nPV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '2'){							//0
		//soigne
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("Type de message : SOIGNE \nLongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nPV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '3'){							//0
		//notification
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		typeDeModification = digit_to_int(mesg[5]);		//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);

		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("Type de message : NOTIFICATION \nLongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nTypeModification : %i PV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, typeDeModification,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '4'){							//0
		//mort
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		//donneesLongueur = getDonneesLongueur(mesg);	//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		printf("Type de message : MORT \nLongueurNomSource : %i LongueurNomCible : %i \nNomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '5'){							//0
		//Liste de client
		printf("Type de message : LISTE DES CLIENTS \n");
		int nbClient;
		nbClient = getNbClient(mesg);					//1 | 2 NBCLIENT
		int longueurNomClient[nbClient];
		//char*
		int offset = 0;
		int i;
		for(i = 0; i < nbClient; ++i){
			offset = 2 * i + 3;
			longueurNomClient[i] = getLongueurNomClient(mesg, offset);
			//printf("longueurNomClient a la position : %i -- %i\n", i, longueurNomClient[i]);
			//sommeLongueur = sommeLongueur + longueurNomClient[i];
		}
		offset = offset + 2;
		char * nomsClients[nbClient];

		for(i = 0; i < nbClient; ++i){
			nomsClients[i] = calloc(longueurNomClient[i]+1, 1);
			strncpy(nomsClients[i], &mesg[offset], longueurNomClient[i]);
			//printf("nomClient a la position : %i -- %s\n", i, nomsClients[i]);
			printf("%i/ %s\n", i+1, nomsClients[i]);
			offset = longueurNomClient[i] + offset;
		}

	}else if(mesg[0] == '6'){							//0
		//Deconnexion
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomSource = calloc(nomSourceLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		printf("Type de message : DECONNEXION \nLongueurNomSource : %i\nNomSource : %s\n",
		 nomSourceLongueur, nomSource);
		free(nomSource);
	}else if(mesg[0] == '7'){							//0
		//Obtenir liste de client
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomSource = calloc(nomSourceLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		printf("Type de message : DEMANDE LISTE DES CLIENTS \nLongueurNomSource : %i\nNomSource : %s\n",
		 nomSourceLongueur, nomSource);
		free(nomSource);
	}else{
		perror("erreur : message errone.");
		exit(1);
	}
	printf("\n");
}



int sendRequeteNBClient(int port, char* host, char* mesg, char* clients[] ) {
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
	int nbClient;
	nbClient = 0;
	while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		nbClient = getNbClient(buffer);
		nbClient = getNbClient(mesg);
		int longueurNomClient[nbClient];
		int offset = 0;
		int i;
		for(i = 0; i < nbClient; ++i){
			offset = 2 * i + 3;
			longueurNomClient[i] = getLongueurNomClient(mesg, offset);
		}
		offset = offset + 2;
		for(i = 0; i < nbClient; ++i){
			clients[i] = calloc(longueurNomClient[i]+1, 1);
			strncpy(clients[i], &mesg[offset], longueurNomClient[i]);
			//printf("nomClient a la position : %i -- %s\n", i, clients[i]);
			printf("%i/ %s\n", i+1, clients[i]);
			offset = longueurNomClient[i] + offset;
		}
		write(1,buffer,longueur);
    }
    printf("\n");
    close(socket_descriptor);
    return nbClient;
}


char * genMessage (int port, char* host, char* nomClient){
	int nbClient;
	char* mesgRequete = calloc(strlen(strcat(strcat (strcat("7", strlen(nomClient)), "000000"), nomClient))+2, 1);
	char * clients[16];
	if(strlen(nomClient)> 9){
		mesgRequete = strcat(strcat (strcat("7", strlen(nomClient)), "000000"), nomClient);
		nbClient = sendRequeteNBClient(port, host, mesgRequete, clients);
	}else{
		mesgRequete = strcat(strcat (strcat("70", strlen(nomClient)), "000000"), nomClient);
		nbClient = sendRequeteNBClient(port, host, mesgRequete, clients);
	}
	printf("Choix cible\n");
	int choix;
	int type;
	type = -1;
	while(type < 0 || type > 2){
		scanf("%i", type);
		if(type < 0 || type > nbClient){
			printf("Le choix doit être entre 0 et %i", 2);	
		}
	}
	choix = -1;
	while(choix < 0 || choix > nbClient){
		scanf("%i", choix);
		if(choix < 0 || choix > nbClient){
			printf("Le choix doit être entre 0 et %i", nbClient);
		}
	}
	char* mesg = calloc(strlen(strcat(strcat (strcat("7", strlen(nomClient)), "000000"), nomClient))+2 , 1);
	if(type == 1){							//0
		
		if(strlen(nomClient)> 9){
			mesg = strcat(strcat (strcat("1", strlen(nomClient)), "000000"), nomClient);
			mesg = strcat("1", strlen(nomClient));
			if(strlen(clients[choix]) > 9){
				mesg = strcat(mesg, strlen(clients[choix]));
			}else{
				mesg = strcat(mesg, "0");
				mesg = strcat(mesg, strlen(clients[choix]));
			}
			mesg = strcat(mesg, "3");
		}else{
			mesg = strcat(strcat (strcat("10", strlen(nomClient)), "000000"), nomClient);
			mesg = strcat("10", strlen(nomClient));
			if(strlen(clients[choix]) > 9){
				mesg = strcat(mesg, strlen(clients[choix]));
			}else{
				mesg = strcat(mesg, "0");
				mesg = strcat(mesg, strlen(clients[choix]));
			}
			mesg = strcat(mesg, "3");

		}
		mesg = strcat(mesg, nomClient);
		mesg = strcat(mesg, clients[choix]);
		mesg = strcat(mesg, "005");
	}else if(type == 2){							//0
		if(strlen(nomClient)> 9){
			mesg = strcat("1", strlen(nomClient));
			if(strlen(clients[choix]) > 9){
				mesg = strcat(mesg, strlen(clients[choix]));
			}else{
				mesg = strcat(mesg, "0");
				mesg = strcat(mesg, strlen(clients[choix]));
			}
			mesg = strcat(mesg, "3");
		}else{
			mesg = strcat("10", strlen(nomClient));
			if(strlen(clients[choix]) > 9){
				mesg = strcat(mesg, strlen(clients[choix]));
			}else{
				mesg = strcat(mesg, "0");
				mesg = strcat(mesg, strlen(clients[choix]));
			}
			mesg = strcat(mesg, "3");

		}
		mesg = strcat(mesg, nomClient);
		mesg = strcat(mesg, clients[choix]);
		mesg = strcat(mesg, "005");
	}else{
		if(strlen(nomClient)> 9){
			mesg = strcat(strcat (strcat("6", strlen(nomClient)), "000000"), nomClient);
		}else{
			mesg = strcat(strcat (strcat("60", strlen(nomClient)), "000000"), nomClient);
		}
	}
	return mesg;

}

int main(){
	char* m1 = calloc(100, 1);
	m1 = "006000003ELDRAD";
	decode(m1);
	//attaque
	//nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
	//nomCibleLongueur = getCibleLongueur(mesg);	//3 | 4
	//typeDeModification inutile					//5
	//donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8

	char* m2 = calloc(100, 1);
	//				  12 15 18
	//				10 13 16 19
	//	  0123456789 11 14 17
	m2 = "106030003ELDRADBOB002";
	decode(m2);

	char* m3 = calloc(100, 1);
	m3 = "206030003EldradBOB002";
	decode(m3);
	char* m4 = calloc(100, 1);
	m4 = "306030003ELDRADBOB002";
	decode(m4);
	char* m5 = calloc(100, 1);
	m5 = "406030000ELDRADBOB";
	decode(m5);
	char* m6 = calloc(100, 1);
	m6 = "506060302100405ELDRADBOBXILUMINACALIJULEFREYA";
	decode(m6);
	char* m7 = calloc(100, 1);
	m7 = "606000000ELDRAD";
	decode(m7);
	char* m8 = calloc(100, 1);
	m8 = "706000000ELDRAD";
	decode(m8);



/*
	free(m1);
	free(m2);
	free(m3);
	free(m4);
	free(m5);
	free(m6);
	free(m7);
	free(m8);
*/
	printf("FIN\n");
}