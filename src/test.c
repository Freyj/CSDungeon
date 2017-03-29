
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

//#3E6EB7


#define TAILLE_MAX_NOM 13
#define MAX_JOUEURS 3
#define TAILLE_BUFFER 256

// pour compter les joueurs
int nbJoueursCourants = 0;
//pour les identifier
int joueurIdCompteur = 0;

//int du socket
static int descripteurSocket; /* socket descriptor */

//int identifiant le joueur dont c'est le tour
//static int joueurTour = 0;

//structures pour les sockets
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

/* Structure pour stocker les infos d'un personnage */
typedef struct _InfoJoueur {
	/* caracteristiques */
	int pv;
	int pvMax;
	int exp;
	int degats;
	int nbTues;
}InfoJoueur;
/* Structure pour stocker les infos d'un client*/
typedef struct _Joueur {
	InfoJoueur info;
	int sock_desc;
	int nouv_sock;
	int joueurId;
	int longueur_adresse_courante;
	struct sockaddr_in adresse_locale;
	struct sockaddr_in adresse_client;
	struct hostent* ptr_hote;
	char* nomJoueur;
	char* bufferAction;
	//1 si c'est le tour du joueur, sinon 0
	int tourEnCours;
}Joueur;
/* Structure des groupes d'ennemis */
typedef struct _Ennemis{
	int pvEn1;
	int pvEn2;
	int pvEn3;
} Ennemis;
/* Structure pour l'info du jeu */
typedef struct _Jeu{
	Ennemis* ennemis;
	int nbTour;
	int port;
	char* nomServeur;
} Jeu;

//joueurs et jeu variables globales
Joueur* joueurs[MAX_JOUEURS];
Jeu* jeu;


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

int getTypeDeModification(char* mesg){
	return getDonnees(mesg, 1, 5, 0, 0);
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







char* genMessageClient(char* nomSource, char* nomDest, int type){

	char* message;
	char* bufferGenMessage = calloc(5, 1);
	if (type == 1) {
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



char* genMessage(char* nomSource, char* nomDest, int type){

	char* message;
	char* bufferGenMessage = calloc(5, 1);
	if (type == 1) {
		//attaquer
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		strcpy (bufferGenMessage, "3");
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
		strcpy (bufferGenMessage, "3");
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
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomSource) + 3 + 1, 1);
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
		//boucle calculant la somme des noms 
		int i;
		int longueur = 0;
		for (i = 0; i < nbJoueursCourants; ++i) {
			longueur = longueur + strlen((char*) joueurs[i]->nomJoueur);
		}
		message = calloc(TAILLE_BUFFER, 1);
		strcpy (bufferGenMessage, "5");
		message = strcat(message, bufferGenMessage);
		for (i = 0; i < nbJoueursCourants; ++i) {
			if(strlen(joueurs[i]->nomJoueur) <= 9){
				strcpy (bufferGenMessage, "0");
				message = strcat(message, bufferGenMessage);
			}
			sprintf(bufferGenMessage, "%zu", strlen(joueurs[i]->nomJoueur) ); 
			message = strcat(message, bufferGenMessage);
		}
		for (i = 0;i < nbJoueursCourants; ++i) {
			message = strcat(message, joueurs[i]->nomJoueur);
		}
		message = strncat(message, "\0", 1);
		//decode(message);
	}else if (type == 8) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomSource)+10, 1);
		strcpy (bufferGenMessage, "8");

		message = strcat(message, bufferGenMessage);

		strcpy (bufferGenMessage, "00");//LongueurSource
		message = strcat(message, bufferGenMessage);

		if(strlen(nomSource) <= 9){
			strcpy (bufferGenMessage, "0");
			message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%zu", strlen(nomSource)); 
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



int main(){
	char* m0 = calloc(100, 1);
	m0 = "006000003ELDRAD";
	decode(m0);
	//attaque
	//nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
	//nomCibleLongueur = getCibleLongueur(mesg);	//3 | 4
	//typeDeModification inutile					//5
	//donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8

	char* m1 = calloc(100, 1);
	//				  12 15 18
	//				10 13 16 19
	//	  0123456789 11 14 17
	m1 = "106030003ELDRADBOB017";
	decode(m1);

	char* m2 = calloc(100, 1);
	m2 = "206030003EldradBOB042";
	decode(m2);
	char* m3 = calloc(100, 1);
	m3 = "306030003ELDRADBOB101";
	decode(m3);
	char* m4 = calloc(100, 1);
	m4 = "406030000ELDRADBOB";
	decode(m4);
	char* m5 = calloc(100, 1);
	m5 = "506060302100405ELDRADBOBXILUMINACALIJULEFREYA";
	decode(m5);
	char* m6 = calloc(100, 1);
	m6 = "606000000ELDRAD";
	decode(m6);
	char* m7 = calloc(100, 1);
	m7 = "706000000ELDRAD";
	decode(m7);
	char* m8 = calloc(100, 1);
	m8 = "806000000ELDRAD";
	decode(m8);

	char* message;
	printf("------------------------------------------------\n genMessage\n");
	printf("------------------------------------------------\n\n");
	
	message = genMessage("ELDRAD", "BOB", 1);
	decode(message);
	
	message = genMessage("ELDRAD", "BOB", 2);
	decode(message);
	
	message = genMessage("ELDRAD", "BOB", 6);
	decode(message);
	
	message = genMessage("ELDRAD", "BOB", 7);
	decode(message);
	
	message = genMessage("ELDRAD", "BOB", 8);
	decode(message);

	printf("------------------------------------------------\n genMessageClient\n");
	printf("------------------------------------------------\n\n");
	
	message = genMessageClient("ELDRAD", "BOB", 1);
	decode(message);
	
	message = genMessageClient("ELDRAD", "BOB", 2);
	decode(message);
	
	message = genMessageClient("ELDRAD", "BOB", 6);
	decode(message);
	
	message = genMessageClient("ELDRAD", "BOB", 7);
	decode(message);
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