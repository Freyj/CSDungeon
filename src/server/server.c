/**
 * @Authors : Charlene Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.9
 * @brief : 
*/
/*----------------------------------------------
Serveur a lancer avant le client
------------------------------------------------*/
#include "server.h"

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

int getTypeMessage(char* mesg){
	return getDonnees(mesg, 1, 0, 0, 0);
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

/*------------------------------------------------------*/
/*------------------------------------------------------*/
char* heure() {
	time_t temps = time(NULL);
	struct tm structTps = *localtime(&temps);
	char *heureString = malloc(20 * sizeof(char));
	sprintf(heureString, "%d:%d:%d", structTps.tm_hour, structTps.tm_min, structTps.tm_sec);
	return heureString;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
void runLog(char* erreur, int threadNb) {
	FILE* file = fopen("logServeur.log", "a");
	if (file == NULL) {
	printf("Erreur d'ouverture de fichier de log!\n");
	exit(1);
	}
	char* time = heure();
	char* thread ="[*Serveur*] : \0";
	fprintf(file, "%s %s : %s \n", thread, time, erreur);
	fclose(file);
	free(time);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
void runLogInt(int erreur, int threadNb) {
	FILE* file = fopen("logServeur.log", "a");
	if (file == NULL) {
	printf("Erreur d'ouverture de fichier de log!\n");
	exit(1);
	}
	char* time = heure();
	char* thread ="[*Serveur*] : \0";
	fprintf(file, "%s %s : %d \n", thread, time, erreur);
	fclose(file);
	free(time);
}

void initServer (int port) {
	runLog("debut d'initialisation", 0);
	//initialisation des adversaires
	Ennemis* groupeEnnemis = malloc(sizeof(Ennemis));
	groupeEnnemis = genEnnemis(groupeEnnemis);
	jeu->ennemis = groupeEnnemis;
	runLog("Pvs de l'ennemi 1 pour test", 0);
	runLogInt(jeu->ennemis->pvEn1, 0);
	//compteur de tours
	jeu->nbTour = 0;
	jeu->port = port;
	jeu->nomServeur = malloc(sizeof(char) * TAILLE_MAX_NOM);
	gethostname(jeu->nomServeur,TAILLE_MAX_NOM);
	runLog("initialisation terminee", 0);
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*
 * @bug : les ennemis 2 et 3 ne sont pas pris en compte
 */
Ennemis* genEnnemis(Ennemis* en) {
	en->pvEn1 = 100;
	en->pvEn2 = 10;
	en->pvEn3 = 10;
	return en;
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*
 * @bug : les ennemis 2 et 3 ne sont pas pris en compte
 */
int ennemisElimines(Ennemis* en) {
	runLog("Check d'ennemis elimines", 50);
	runLogInt(en->pvEn1, 50);
	if ((en->pvEn1 < 1) /*&& (en->pvEn2 < 1) && (en->pvEn3 < 1)*/) {
		return 1;
		runLog("ennemis elimines", 50);
	}
	return 0;
	runLog("ennemis non elimines", 50);
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
void initJoueur(int sock, char* nom, sockaddr_in adresse_client_courant) {
		//printf("%d\n", sock);
		//printf("%s\n", strerror(errno));
		Joueur* joueur = malloc(sizeof(Joueur));
		joueur->adresse_client =adresse_client_courant;
		joueur->nomJoueur = nom;
		joueur->sock_desc = sock;
		joueur->pv = 100;
		joueur->degats = 5;
		joueur->joueurId = nbJoueursCourants;
		joueurs[nbJoueursCourants] = joueur;
		nbJoueursCourants++;
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
void envoiTous(char* message){
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (joueurs[i]) {
			if (write(joueurs[i]->sock_desc,message,strlen(message)+1) == -1) {
				runLog("Erreur d'envoi pour le client", 50);
				runLog(joueurs[i]->nomJoueur, 50);
			}	
			else {
				runLog("Envoi correct pour le client", 50);
				runLog(joueurs[i]->nomJoueur, 50);
			}
		}
	}
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
int joueursMorts() {
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (joueurs[i]) {
			if (joueurs[i]->pv > 0) {
				return 0;
			}
		}
	}
	return 1;
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
char* action(char* buffer, Joueur* joueurCourant) {
	char* res;
	res = calloc(256,1);
	int type = getTypeMessage(buffer);
	//un client se co
	if (type == 0) {
		//envoi de message generique?
		//res = genMessage()
		printf("Une connexion en cours.\n");
	}
	//un client attaque
	else if (type == 1) {
		res = genMessage(joueurCourant->nomJoueur, getCibleNom(buffer, 9), 1);
		attaque(joueurCourant, getCibleNom(buffer, 9));
		printf("%s\n", "attaque du client" );
	}
	//un client soigne
	else if (type == 2) {
		res = genMessage(joueurCourant->nomJoueur, getCibleNom(buffer, 9), 2);
		soin(joueurCourant, getCibleNom(buffer, 9));
		printf("%s\n", "soin du client" );
	}
	//un client deco
	else if (type == 6) {
		joueurCourant->pv = 0;
		//envoi de message de deco
		res = genMessage(joueurCourant->nomJoueur, "ALL", 6);
		printf("%s\n", "une deconnexion");
	}
	//on demande la liste des clients
	else if (type == 7){
		printf("%s\n", "une liste de clients demandée");
		res = genMessage(joueurCourant->nomJoueur, "ALL", 7);
	}
	else {
		res = calloc(10, 1);
		res = "000000000";
		printf("Zut, y a une erreur.\n");
	}
	return res;
}


/*------------------------------------------------------*/
/*------------------------------------------------------*/

char* genMessage(char* nomSource, char* nomDest, int type){
	//requete liste client
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
		//valeur de dégats codée en dur à 5 pv
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);
		decode(message);
	}
	else if (type == 2) {
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
		//valeur de soin codée en dur à 5 pv
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);
		decode(message);
	}
	else if (type == 6) {	
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
		decode(message);
	}
	else if (type == 7){
		//boucle calculant la somme des noms 
		int i;
		int longueur = 0;
		for (i = 0; i < nbJoueursCourants; ++i) {
			longueur = longueur + strlen((char*) joueurs[i]->nomJoueur);
		}
		message = calloc(TAILLE_BUFFER, 1);
		strcpy (bufferGenMessage, "5");
		message = strcat(message, bufferGenMessage);
		if(nbJoueursCourants <=  9){
            strcpy (bufferGenMessage, "0");
            message = strcat(message, bufferGenMessage);
		}
		sprintf(bufferGenMessage, "%i",  nbJoueursCourants); 
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
		decode(message);
	}
	else if (type == 8) {
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
		decode(message);
	}
	free(bufferGenMessage);
	return message;
}


/*------------------------------------------------------*/
/*------------------------------------------------------*/

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

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * enlève 5 pv à un joueur ou un ennemi
 */
void attaque(Joueur* joueurCourant, char* nomCible) {
	if (joueurExists(nomCible) != NULL) {
		Joueur* cibleJoueur = joueurExists(nomCible);
		cibleJoueur->pv -= 5;
		printf("Attaque réussie\n");
	}
	else if (!strcmp(nomCible, "ennemi")){
		int degats = joueurCourant->degats;
		if (ennemisElimines(jeu->ennemis) == 0) {
			if (&jeu->ennemis->pvEn1 > 0 ) {
				jeu->ennemis->pvEn1 = jeu->ennemis->pvEn1 - degats;
				runLog("ennemi 1 tape", 50);
			}
			else if(&jeu->ennemis->pvEn2 > 0 ) {
				jeu->ennemis->pvEn2 = jeu->ennemis->pvEn2 - degats;
				runLog("ennemi 2 tape", 50);
			}
			else if(& jeu->ennemis->pvEn3 > 0 ) {
				jeu->ennemis->pvEn3 = jeu->ennemis->pvEn3 - degats;
				runLog("ennemi 3 tape", 50);
			}
		}
	}
	else {
		printf("Attaque échouée\n");
	}
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/** 
 * donne 5 pv au joueur s'il existe
 */
void soin(Joueur* joueurCourant, char* nomCible){
	if (joueurExists(nomCible) != NULL) {
		Joueur* cibleJoueur = joueurExists(nomCible);
		cibleJoueur->pv += 5;
		printf("Soin réussi\n");
	}
	else {
		printf("Fail soin\n");
	}
}


/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * renvoie le joueur s'il existe un joueur avec ce nom
 */
Joueur* joueurExists(char* nom) {
	int i;
	for (i = 0; i < nbJoueursCourants; ++i) {
		if (!strcmp(nom, joueurs[i]->nomJoueur)) {
			printf("Joueur existant : %s\n", nom);
			return joueurs[i];
		}
	}
	return NULL;
}
















int main(int argc, char** argv) {
	if (argc != 2) {
		perror("La bonne commande est ./csServ [port]");
	}
	else {
		jeu = malloc(sizeof(Jeu));
		initServer(atoi(argv[1]));

		int socket_descriptor;
		int new_socket_descriptor,  /* new socket descriptor */
	    longueur_adresse_courante; /* client address length */
		sockaddr_in local_address,    /* local address socket informations */
	    adresse_client_courant;  /* client address */
		hostent* ptr_host;  /* informations about host */
		char host_name[TAILLE_MAX_NOM]; /* host name */

		gethostname(host_name,TAILLE_MAX_NOM);  /* getting host name */

		/* set a timeout for 1 min */
		timeoutRead = malloc(sizeof(timeval));
		long int tv_sec = READ_TIMEOUT;
		timeoutRead->tv_sec = tv_sec;
		timeoutRead->tv_usec = 0;

		/* get hostent using server name */
		if ((ptr_host = gethostbyname(host_name)) == NULL) {
	    	perror("error: unable to find server using its name.");
	    	exit(1);
	  	}

	  	/* initialize sockaddr_in with these informations */
	  	bcopy((char*)ptr_host->h_addr, (char*)&local_address.sin_addr, ptr_host->h_length);
		local_address.sin_family = ptr_host->h_addrtype;
		/* AF_INET */
		local_address.sin_addr.s_addr = INADDR_ANY;
		/* use the defined port */
		//local_address.sin_port = htons(atoi(argv[1]));
		local_address.sin_port = htons(atoi(argv[1]));
		printf("Using port : %d \n", ntohs(local_address.sin_port));
		/* create socket in socket_descriptor */

		if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("error: unable to create the connection socket.");
			exit(1);
		  }
		/* bind socket descripteurSocket to sockaddr_in local_address */
		if ((bind(socket_descriptor, (sockaddr*)(&local_address), sizeof(local_address))) < 0) {
			perror("error: unable to bind the socket to the connection address.");
			exit(1);
		}
	  	/* initialize the queue */
	  	listen(socket_descriptor,MAX_JOUEURS);

		printf("Server initialise\n");
		//printf("%d \n",local_address.sin_port);
	  	//on attend la connexion de tous les joueurs
	  	while(nbJoueursCourants < MAX_JOUEURS) {
	  		printf("ajouts de joueurs\n");
			longueur_adresse_courante = sizeof(adresse_client_courant);
			if ((new_socket_descriptor = accept(socket_descriptor, (sockaddr*) (&adresse_client_courant),(socklen_t *) &longueur_adresse_courante)) < 0) {
				perror("erreur : impossible d'accepter la connexion avec le client.");
				exit(1);
			}
			else if (new_socket_descriptor == 0) {
				//printf("%s\n", "AHHHHHHHHHHHHHHHHHHH");
			}
			int longueur;
			char buffer[TAILLE_BUFFER];
			memset(buffer, '0', TAILLE_BUFFER);
			longueur = read(new_socket_descriptor, buffer, TAILLE_BUFFER-1);
			//runLogInt(errno,1);
			//printf("%s\n", strerror(errno));
			if (longueur < 0) {
				printf("erreur de lecture\n");
			}
			else if (longueur == 0) {
				printf("Disconnect du client\n");		
			}
			if (longueur > 0) {
				printf("%d\n", longueur);
				//initialise le joueur ET incrémente le nbJoueursCourant. (ajoute dans le tableau)
				//init de nom pourri pour tes
				printf("reception\n");
				printf("%s\n", buffer);
				printf("nom du client qui se connecte\n");
				printf("%s\n", getSourceNom(buffer, 9));
				printf("%s\n", "fin reception");
				initJoueur(new_socket_descriptor, getSourceNom(buffer, 9), adresse_client_courant);
			}
		}

		//pour un envoi de la liste des clients, on s'en fiche de la source/cible, en plus c'est un broadcast
		//le serveur renvoie la liste des clients
		envoiTous(genMessage("", "", 7));
		//sleep pour eviter des receptions ratés
		sleep(1);
		int tourCpt;
		for (tourCpt = 0; tourCpt < MAX_TOURS ; ++tourCpt) {
			printf("%d\n", tourCpt);
			//on verifie que tous les joueurs sont pas morts
			if(joueursMorts() == 0) {
				int iterJoueur;
				//on parcourt les joueurs dans l'ordre du tableau
				for (iterJoueur = 0; iterJoueur < nbJoueursCourants; ++iterJoueur) {
					printf("iteration de joueur\n");
					//on vérifie que le joueur existe
					if (joueurs[iterJoueur]) {
						printf("joueur %d\n", iterJoueur);
						printf("%s\n", joueurs[iterJoueur]->nomJoueur);
						int longueur = 0;
						char buffer[TAILLE_BUFFER];
						//on écoute
						//segfault
						//printf("%d\n", joueurs[iterJoueur]->sock_desc);
						//setsockopt(joueurs[iterJoueur]->sock_desc, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&timeoutRead,sizeof(struct timeval));
						//envoi du token au joueur en question
						//generation du message, de type 8 (token), 0 parce qu'on s'en fiche
						//50 = max size (large)
						char* msg = calloc(TAILLE_BUFFER, sizeof(char));
						msg = genMessage(joueurs[iterJoueur]->nomJoueur,joueurs[iterJoueur]->nomJoueur, 8);
						//printf("(%s\n", "BOUH");
						printf("%s\n", msg);
						envoiTous(msg);
						longueur = read(joueurs[iterJoueur]->sock_desc, buffer, TAILLE_BUFFER-1);
						//runLogInt(errno,1);
						//printf("%s\n", strerror(errno));
						if (longueur < 0) {
							printf("erreur de lecture\n");
							joueurs[iterJoueur] = NULL;
						}
						else if (longueur == 0) {
							printf("fermeture d'un socket\n");
							joueurs[iterJoueur] = NULL;							
						}
						if (longueur > 0) {
							//printf("%s\n", buffer);
							//si le joueur n'est pas mort
							//printf("a reçu un message\n");
							if (joueurs[iterJoueur]->pv > 0) {
								//printf("joueur vivant\n");
								printf("PV du joueur %i\n", joueurs[iterJoueur]->pv);
								//si les ennemis sont morts, on envoie un message de fin
								//TODO: plus tard, on enverra un truc pour redémarrer?
								if (ennemisElimines(jeu->ennemis) == 1) {
									//printf("VICTOIRE\n");
									//message de fin (le 9 envoie fin de jeu, le cinquième caractere determine vitoire
									//ou défaite (1 ou 0))
									char* bufferFinJeu = "900001";
									envoiTous(bufferFinJeu);
									printf("victoire des joueurs\n");
									return 0;
								}
								else {
									//printf("Il se passe quelque chose\n");
									buffer[longueur] = '\0';
									runLog(buffer, 0);
									joueurs[iterJoueur]->bufferAction = buffer;
									char* bufferRenvoi;
									//on traite l'action liée au buffer
									bufferRenvoi = action(buffer, joueurs[iterJoueur]);
									//on broadcast à tout le monde l'info
									envoiTous(bufferRenvoi);
								}
							}
							//on ignore les joueurs morts
						}
						//printf("DID NOT READ\n");
					}
					sleep(1);
					//le joueur existe pas on passe au suivant (pas opti)
				}
			}
			else {
				//si ils sont tous morts, on envoi un message de défaite
				//message de fin (le 9 envoie fin de jeu, le cinquième caractere determine vitoire
				//ou défaite (1 ou 0))
				char* bufferFinJeu = "900000";
				envoiTous(bufferFinJeu);
				printf("defaite des joueurs\n");
				return 0;
			}
		}
	}
	return 0;
}