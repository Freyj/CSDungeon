/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.3
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
#include <pthread.h>
#include <unistd.h>

#define TAILLE_BUFFER 1024

/*------------------------------------------------------------------------------*/

void* Ecoute(void* arg);
char* genMessage(char* nomSource, char* nomDest, int type);
int sendRequeteNBClient(int port, char* host, char* mesg, char* clients[] );
void decode(char* mesg);
int getDonnees(char* mesg, int nbData, int data1Pos, int data2Pos, int data3Pos);
int getSourceLongueur(char* mesg);
int getCibleLongueur(char* mesg);
int getDonneesLongueur(char* mesg);
int getPointsDeVie(char* mesg, int offset);
int getNbClient(char* mesg);
int getLongueurNomClient(char* mesg, int offset);
int getTypeDeModification(char* mesg);
char* getSourceNom(char* mesg, int longueurEntete);
char* getCibleNom(char* mesg, int longueurEntete);
void sendMessage(int socket_descriptor, char* mesg);

/*------------------------------------------------------------------------------*/

typedef struct sockaddr 
sockaddr;

typedef struct sockaddr_in 
sockaddr_in;

typedef struct hostent 
hostent;

typedef struct servent 
servent;
//0 = c'est pas mon tour
static int estMonTour = 0;

/* Structure pour stocker les infos du client */
typedef struct infoclient {
	char* nom;
	int pv;
	int force;
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
	if(nbData == 1){
		dataToReturn = dataToReturn + digit_to_int(mesg[data1Pos]);
	}else if(nbData == 2){
		dataToReturn = dataToReturn + 10 * digit_to_int(mesg[data1Pos]);
		dataToReturn = dataToReturn + digit_to_int(mesg[data2Pos]);
	}else{
		dataToReturn = dataToReturn + 100 * digit_to_int(mesg[data1Pos]);
		dataToReturn = dataToReturn + 10 * digit_to_int(mesg[data2Pos]);
		dataToReturn = dataToReturn + digit_to_int(mesg[data3Pos]);
	}
	return dataToReturn;
}

char* getSourceNom(char* mesg, int longueurEntete) {
	int nomSourceLongueur = getSourceLongueur(mesg);
	char* nomSource = calloc(nomSourceLongueur+1, 1);
	strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
	return nomSource;
}

char* getCibleNom(char* mesg, int longueurEntete) {
	int nomCibleLongueur = getCibleLongueur(mesg);
	char* nomCible = calloc(nomCibleLongueur+1, 1);
	strncpy(nomCible, &mesg[longueurEntete + getSourceLongueur(mesg)], nomCibleLongueur);
	return nomCible;
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
//offset : taille entete + longueur nom de la source + longueur nom de la cible
int getLongueurNomClient(char* mesg, int offset){
	return getDonnees(mesg, 2, offset, offset + 1, 0);
}

int getTypeDeModification(char* mesg){
	return getDonnees(mesg, 1, 5, 0, 0);
}

int getTypeMessage(char* mesg){
	return getDonnees(mesg, 1, 0, 0, 0);
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
	int estVictoire;
	longueurEntete = 9;
	printf("\t\t");
	printf("%s", mesg);
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
		typeDeModification = getTypeDeModification(mesg);	//5
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
		printf("Type de message : DECONNEXION \nLongueurNomSource : %i\nNomClientDeco : %s\n",
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
	}else if(mesg[0] == '8'){
		//estMonTour = 1;
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		//donneesLongueur = getDonneesLongueur(mesg);	//6 | 7 | 8
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomCible, &mesg[longueurEntete], nomCibleLongueur);
		printf("Type de message : C'EST AU TOUR DE ! \n longueurNomClient : %i\nnomCible : %s\n",
		 nomCibleLongueur, nomCible);
		//cest a ton tour
		//perror("erreur : type de message non definit pour le moment.");
		//exit(1);
	}else if(mesg[0] == '9'){
		//Fin du Jeu
		estVictoire = getTypeDeModification(mesg);
		printf("Type de message : FIN DE JEU \n estVictoire : %i",
		 estVictoire);
	}else{
		perror("erreur : message errone.");
		exit(1);
	}
	printf("\n");
}

void sendMessage(int socket_descriptor, char* mesg) {
	if ((write(socket_descriptor, mesg, strlen(mesg)+1)) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.\n");
		exit(1);
	}
	printf("message envoye au serveur. \n");
	printf("%s\n", mesg);
}

char* genMessage(char* nomSource, char* nomDest, int type){

	char* message;
	char* bufferGenMessage = calloc(5, 1);
	if (type == 0){
		//connexion
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "0");
		message = strcat(message, bufferGenMessage);
		//longueur des noms
		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
		message = strcat(message, bufferGenMessage);
		
		strcpy (bufferGenMessage, "00");//LongueurSource
		message = strcat(message, bufferGenMessage);
		
		strcpy (bufferGenMessage, "0");//TDM
		message = strcat(message, bufferGenMessage);
		strcpy (bufferGenMessage, "000");//LongueurDonnees
		message = strcat(message, bufferGenMessage);

		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, "\0", 1);
		
	}else if (type == 1) {
		//attaquer
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "1");
		message = strcat(message, bufferGenMessage);
		//longueur des noms
		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
		message = strcat(message, bufferGenMessage);

		if(strlen(nomDest) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomDest)); 
		message = strcat(message, bufferGenMessage);

		//concat
		message = strncat(message, "0", 1);
		message = strncat(message, "003", 3);
		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, nomDest, strlen(nomDest));
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);
		//decode(message);
	}else if (type == 2) {
		//soigner
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "2");
		message = strcat(message, bufferGenMessage);
		//longueur des noms
		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
		message = strcat(message, bufferGenMessage);

		if(strlen(nomDest) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomDest)); 
		message = strcat(message, bufferGenMessage);

		//concat
		message = strncat(message, "1", 1);
		message = strncat(message, "003", 3);
		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, nomDest, strlen(nomDest));
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);
		//decode(message);
	}else if (type == 6) {	
		//deconnexion
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "6");
		message = strcat(message, bufferGenMessage);	
		//longueur des noms
		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
		message = strcat(message, bufferGenMessage);
		
		strcpy (bufferGenMessage, "00");//LongueurSource
		message = strcat(message, bufferGenMessage);
		strcpy (bufferGenMessage, "0");//TDM
		message = strcat(message, bufferGenMessage);
		strcpy (bufferGenMessage, "000");//LongueurDonnees
		message = strcat(message, bufferGenMessage);

		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, "\0", 1);
		//decode(message);
	}else if (type == 7){
		//Demande noms des clients
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "7");
		message = strcat(message, bufferGenMessage);	
		//longueur des noms
		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
		message = strcat(message, bufferGenMessage);
		
		strcpy (bufferGenMessage, "00");//LongueurSource
		message = strcat(message, bufferGenMessage);
		strcpy (bufferGenMessage, "0");//TDM
		message = strcat(message, bufferGenMessage);
		strcpy (bufferGenMessage, "000");//LongueurDonnees
		message = strcat(message, bufferGenMessage);

		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, "\0", 1);
		//decode(message);
	}
	free(bufferGenMessage);
	return message;
}

void transmissionDonneesinitiale(int socket_descriptor, char* nomClient ) {
	printf("envoi initial\n");
	printf("%s\n", nomClient);
	char* buffer = genMessage(nomClient, "bob", 0);
	printf("%s\n", buffer);
	sendMessage(socket_descriptor, buffer);
	printf("envoi initial terminé\n");
}

char* makeClientMessage(char* nomClient) {
	int returnTypeMessage;
	char* message = calloc(256, 1);
	char* cible = calloc(13, 1);
	int invalidInput = 1;
	while(invalidInput == 1) {
		printf("Que faites-vous ?\n1 pour attaquer\n2 pour soigner\n3 pour quitter\n");
		int tmpInt;
		if (scanf ("%d",&tmpInt) == 1 ){
			printf("1\n");
			returnTypeMessage = tmpInt;
			invalidInput = 0;
		}
		else{
			printf("Saisie invalide.");
		}
	}
	if (returnTypeMessage == 3) {
		message = genMessage(nomClient, nomClient, 6);
	}
	else {
		printf("Quelle cible?\n");
		scanf("%s", cible);
		message = genMessage(nomClient, cible, returnTypeMessage);
	}
	return message;
}

































int main(int argc, char **argv) {	
	int socket_descriptor;
	/* descripteur de socket */
	//	int longueur; 				/* longueur d'un buffer utilisé */

	//	sockaddr_in adresse_locale; /* adresse de socket local */

	//	hostent* ptr_host;  		/* info sur une machine hote */
	//	servent* ptr_service;  		/* info sur service */
	//	char buffer[256];
	char* prog; 				/* nom du programme */
	hostent* ptr_host; 				/* nom de la machine distante */
	char* mesg = malloc(256); 	/* message envoyé */
	char* nomClient;
	int numeroPort;
	int deconnexion;
	char* host;
	/* adresse_client_courant sera renseignee par accept via les infos du connect */
	//on assigne le nom du joueur sur le message
	char buffer[256];
	int longueur;
	//int new_socket_descriptor, /* [nouveau] descripteur de socket */
	int longueur_adresse_courante; /* longueur d'adresse courante d'un client */
	
	sockaddr_in 
		adresse_locale, /* structure d'adresse locale*/
		adresse_client_courant; /* adresse client courant */




	if (argc != 4) {
		perror("usage : client <adresse-serveur> <numero-port> <client-name>\n");
		exit(1);
	}

	prog = argv[0];
	host = argv[1];
	numeroPort = atoi(argv[2]);
	nomClient = argv[3];

	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);
	printf("nom du client  : %s \n", nomClient);

	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.\n");
		exit(1);
	}
	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

	//adresse_locale.sin_port = htons(7332); // why port 7332 ?
	adresse_locale.sin_port = htons(numeroPort);

	printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.\n");
		exit(1);
	}

	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.\n");
		exit(1);
	}
	longueur_adresse_courante = sizeof(adresse_client_courant);


	/*
	if ((longueur = recv(socket_descriptor, buffer, sizeof(buffer))) <= 0) {
		return (void*) 1;
	}*/


	transmissionDonneesinitiale(socket_descriptor, nomClient );

	deconnexion = 0;
	estMonTour = 0;
	while(deconnexion == 0){

		if(estMonTour == 0){
			//je lis
			if((longueur = read(socket_descriptor, buffer, TAILLE_BUFFER-1)) > 0) {
				printf("reponse du serveur : \n");
				buffer[longueur] = '\0';
				printf("%s\n", buffer);
				//write(1,buffer,longueur+1);
				int typMess = getTypeMessage(buffer);
				printf("%d\n", typMess);
				//si on a gagné ou perdu
				if (typMess == 9) {
					if (getTypeDeModification(buffer) == 1) {
						printf("Bravo, vous avez gagne la partie\n");
					}
					else {
						printf("Pas de bol, vous avez perdu la partie\n");
					}
					deconnexion = 1;
					break;
				}
				//un message annoncant une mort 
				else if ((typMess == 4)) {
					if (!strcmp(getCibleNom(buffer, 9), nomClient)) {
						printf("Vous etes malheureusement mort.\n");
						printf("Si vous restez connectes, vous pourrez voir le reste du combat\n");
					}
					else {
						//int nomCibleLongueur, char* mesg, int longueurEntete (9 sauf pour liste)
						printf("%s est mort.\n", getCibleNom(buffer, 9));
					}
				}
				//message de deco
				else if (typMess == 6){
					if ((!strcmp(getCibleNom(buffer, 9), nomClient)) || (!strcmp(getSourceNom(buffer, 9), nomClient))) {
						printf("testCMP DECO\n");
					}
					else {
						printf("testCMP DECO ELSE\n");
					}
					printf("Deconnexion\n");
					deconnexion = 1;
					break;
				}
				//message de c'est ton tour
				else if (typMess == 8)  {
					/*printf("%s\n", "TEST AFFICHAGE BUFFER");
					printf("%s\n", buffer);
					printf("%s\n", "FIN TEST AFFICHAGE BUFFER");
					printf("%s\n", "TEST AFFICHAGE NOM");
					printf("%s\n", getCibleNom(buffer, 9));
					printf("%s\n", nomClient);
					printf("%s\n", "FIN TEST AFFICHAGE NOM");*/
					if ((!strcmp(getCibleNom(buffer, 9), nomClient))) {
						printf("C'est mon tour\n");
						estMonTour = 1;
					}
					else {
						printf("%s\n", "C'est pas mon tour!\n");
					}
				}
				//message de liste de clients
				else if (typMess == 7) {
					printf("%s\n", "liste des clients reçus\n");
				}
				else {
					printf("Message reçu \n");
					decode(buffer);
				}

				//printf("Message reçu.\n");
			}
			else {
				printf("erreur lecture \n");
				break;
			}
		}
		//CEST MON TOUR
		else {
			char* mesg = malloc(sizeof(char) * TAILLE_BUFFER);
			mesg = makeClientMessage(nomClient);
			printf("message envoyé\n");
			printf("%s\n", mesg);
			sendMessage(socket_descriptor, mesg);
			estMonTour = 0;
		}
	}

	free(mesg);
	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	exit(0);  
}
