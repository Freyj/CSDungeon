/**
 * @Authors : Charlene Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.2
 * @brief : 
*/
/*----------------------------------------------
Serveur a lancer avant le client
------------------------------------------------*/
#include "server.h"

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction qui donne l'heure pour les logs
 * @details renvoie l'heure en hh:mm:ss
 * @return un char* decrivant l'heure
 */
char* heure() {
	time_t temps = time(NULL);
	struct tm structTps = *localtime(&temps);
	char *heureString = malloc(20 * sizeof(char));
	sprintf(heureString, "%d:%d:%d", structTps.tm_hour, structTps.tm_min, structTps.tm_sec);
	return heureString;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction pour logger les erreurs dans un fichier
 * @param erreur le message en char*
 * @param threadNb le nb du thread pour debug 
 */
void runLog(char* erreur, int threadNb) {
	FILE* file = fopen("logServeur.log", "a");
	if (file == NULL) {
	printf("Erreur d'ouverture de fichier de log!\n");
	exit(1);
	}
	char* time = heure();
	char* thread = "Thread";
	if (threadNb == 0){
		thread = "[*Serveur*][main Thread]:";
	}
	else if(threadNb == 1){
		thread = "[*Serveur*] [*Thread 1*]:";
	}
	else if(threadNb == 2){
		thread = "[*Serveur*] [*Thread 2*]:";
	}
	else if(threadNb == 3){
		thread = "[*Serveur*] [*Thread 3*]:";
	}
	fprintf(file, "%s %s : %s \n", thread, time, erreur);
	fclose(file);
	free(time);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction pour logger les erreurs dans un fichier
 * fonctionne pour les int
 * @param erreur le message en int
 * @param threadNb le nb du thread pour debug 
 */
void runLogInt(int erreur, int threadNb) {
	FILE* file = fopen("logServeur.log", "a");
	if (file == NULL) {
	printf("Erreur d'ouverture de fichier de log!\n");
	exit(1);
	}
	char* time = heure();
	char* thread = "Thread";
	if (threadNb == 0){
		thread = "[**Serveur**][main Thread]:";
	}
	else if(threadNb == 1){
		thread = "[**Serveur**] [*Thread 1*]:";
	}
	else if(threadNb == 2){
		thread = "[**Serveur**] [*Thread 2*]:";
	}
	else if(threadNb == 3){
		thread = "[**Serveur**] [*Thread 3*]:";
	}
	else {
		thread = "[**Serveur**][*No thread*]:";
	}
	fprintf(file, "%s %s : %d \n", thread, time, erreur);
	fclose(file);
	free(time);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction de renvoi
 */
void renvoi(int sock) {
	char buffer[256];
	int longueur;
	if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) {
		return;
	}
	runLog(buffer, 50);
	printf("message lu: %s \n", buffer);

	memset(&buffer, 0, 1);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction generant un groupe d'ennemis
 * @details 3 ennemis a 20 pv, pas encore de reglages
 */
Ennemis* genEnnemis(Ennemis* en) {
	en->pvEn1 = 200;
	en->pvEn2 = 200;
	en->pvEn3 = 200;
	return en;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction verifiant qu'un groupe n'est pas mort
 */
int ennemisElimines(Ennemis* en) {
	runLog("Check d'ennemis elimines", 50);
	runLogInt(en->pvEn1, 50);
	if ((en->pvEn1 < 1) && (en->pvEn2 < 1) && (en->pvEn3 < 1)) {
		return 1;
	}
	return 0;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction lançant une attaque sur un groupe
 */
void attaque(int degats) {
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
	runLog("ennemis tues", 50);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction qui soigne
 * TODO: do (de 2)
 */
void soigner() {
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/** @brief fonction qui enleve un joueur et decremente le total
 *
 */
void retraitJoueur(Joueur* joueur){
	int i;
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (joueurs[i]) {
			joueurs[i] = NULL;
			break;
		}
    }
  nbJoueursCourants--;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/** 
 * Fonction qui gere le tour
 * @TODO: purger et faire des fonctions
 */ 
void* tourDeJeu(void* arg) {
	while(jeu->nbTour < 20) {
		//on verrouille le jeu pour pas qu'un client le modifie entre temps
		pthread_mutex_lock(&jeu->mutex_Jeu);
			if (joueurs[joueurTour]) {
				//le joueur existe, on verrouille pour pouvoir faire quelque chose
				pthread_mutex_lock(&joueurs[joueurTour]->mutex_Joueur);
				runLog("Joueur recu", 15);
				decode(joueurs[joueurTour]->bufferAction);
				char* message;
				if(joueurs[joueurTour]->bufferAction[0] == '7') {
					message = genMessage(jeu->port, jeu->nomServeur, "serveur", "rien", 7, 0);
				}
				else if (joueurs[joueurTour]->bufferAction[0] == '1') {
					int nomSourceLongueur;
					int nomCibleLongueur;
					int donneesLongueur;
					char* nomCible;
					//attaque
					nomSourceLongueur = getSourceLongueur(joueurs[joueurTour]->bufferAction);	//1 | 2
					nomCibleLongueur = getCibleLongueur(joueurs[joueurTour]->bufferAction);		//3 | 4
					//typeDeModification inutile					//5
					donneesLongueur = getDonneesLongueur(joueurs[joueurTour]->bufferAction);
					nomCible = calloc(nomCibleLongueur+1, 1);
					strncpy(nomCible, &joueurs[joueurTour]->bufferAction[9 + nomSourceLongueur], nomCibleLongueur);
					message = genMessage(jeu->port, jeu->nomServeur, joueurs[joueurTour]->nomJoueur, nomCible, 1, 0);
				}
				else if (joueurs[joueurTour]->bufferAction[0] == '2') {
					int nomSourceLongueur;
					int nomCibleLongueur;
					int donneesLongueur;
					char* nomCible;
					//attaque
					nomSourceLongueur = getSourceLongueur(joueurs[joueurTour]->bufferAction);	//1 | 2
					nomCibleLongueur = getCibleLongueur(joueurs[joueurTour]->bufferAction);		//3 | 4
					//typeDeModification inutile					//5
					donneesLongueur = getDonneesLongueur(joueurs[joueurTour]->bufferAction);
					nomCible = calloc(nomCibleLongueur+1, 1);
					strncpy(nomCible, &joueurs[joueurTour]->bufferAction[9 + nomSourceLongueur], nomCibleLongueur);
					message = genMessage(jeu->port, jeu->nomServeur, joueurs[joueurTour]->nomJoueur, nomCible, 2, 1);
				}
				envoiTous(message);
				pthread_mutex_unlock(&joueurs[joueurTour]->mutex_Joueur);
			}
			else {
				//si on a pas de joueur, on incrémente l'id (jusqu'à 16, après on le remet à )
				if (joueurTour <16) {
					joueurTour++;
				}
				else {
					joueurTour = 0;
					++jeu->nbTour;
				}
			}
		pthread_mutex_unlock(&jeu->mutex_Jeu);
	}
	return NULL;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction reagissant a l'action d'un client
 */ 
void action(int sock, Ennemis* en) {
	char buffer[256];
	int longueur;
	if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) {
		return;
	}
	//on previent le client qu'il a fait quelque chose
	//char* reponse;

	// if (strcmp(buffer, "Attaque") == 0) {
	// 	//attaque(en, 2);
	// 	reponse = "Vous avez attaque";
	// 	write(sock,reponse,strlen(reponse)+1);	
	// }
	// if (strcmp(buffer, "Soigner") == 0) {
	// 	soigner();
	// 	reponse = "Vous vous etes soignes";
	// 	write(sock,reponse,strlen(reponse)+1);	
	// }
	//0 est un int, et equivalent a \0
	memset(&buffer, 0, 1);
}
/**
 *	@brief initialisation du serveur
 */ 
void initServer(int port) {
	//comme on modifie le jeu
	runLog("debut d'initialisation", 0);
	//theoriquement, le mutex n'est pas necessaire ici, mais au cas où
	//init du mutex 
	pthread_mutex_lock(&jeu->mutex_Jeu);
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

	pthread_mutex_unlock(&jeu->mutex_Jeu);
	runLog("initialisation terminee", 0);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief envoie un message a tous les joueurs
 * @details log les envois
 * @param message le char* du message
 */
void envoiTous(char* message){
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (joueurs[i]) {
			if (write(joueurs[i]->nouv_sock,message,strlen(message)+1) == -1) {
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
/**
 * @brief gere les signaux 
 * @param nomSignal : l'int du signal (SIGINT)
 */
void gestionSignal(int nomSignal){
  int i;
  runLog("Signal reçu:", 50);
  runLog(strsignal(nomSignal), 50);
  /* Warn the clients that the server is closing */
  if (nomSignal == SIGINT || nomSignal == SIGTERM) {
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (joueurs[i]) {
			envoiTous("Deconnexion Serveur.");
	  		close(joueurs[i]->nouv_sock);
	  		free(joueurs[i]);
		}
	}
    close(descripteurSocket);
  }
  exit(nomSignal);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction par client 
 */
void* loopJoueur(void* arg){
	Joueur* joueur = (Joueur*) arg;
	char* buffer = NULL;
	int longueur;
	while ((longueur = read(joueur->nouv_sock, buffer, TAILLE_BUFFER)) > 0){
	/* évite les soucis de buffer */
	buffer[longueur] = '\0';
	//on modifie le buffer donc on demande le mutex
	pthread_mutex_lock(&joueur->mutex_Joueur);
	joueur->bufferAction = buffer;
	pthread_mutex_unlock(&joueur->mutex_Joueur);
}
  	/* Client quit/disconnected */
  	/* Notify the clients */
  	runLog("Le joueur a quitte : ", 2);
  	runLog(joueur->nomJoueur, 2);
  	char* message = malloc(sizeof(char) * 35);
  	message = strcat("Le joueur a quitte : ", joueur->nomJoueur);
  	envoiTous(message);
  	close(joueur->nouv_sock);
  	retraitJoueur(joueur);
  	
  	free(joueur);
  	free(buffer);
  	pthread_detach(pthread_self());
	return NULL;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/** 
 * @brief fonction qui ajoute un joueur et incremente le total
 */
void ajoutJoueur(Joueur* joueur) {
	int i;
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (!(&joueurs[i])) {
			joueurs[i] = joueur;
			break;
		}
		}
		nbJoueursCourants++;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief  Initialisation du joueur
 */
Joueur* initJoueur(sockaddr_in adresse_locale, int nouv_sock) {
	Joueur* joueur = (Joueur *)calloc((sizeof(Joueur)), 1);
	joueur->adresse_locale = adresse_locale;
	joueur->nouv_sock = nouv_sock;
	joueur->joueurId = joueurIdCompteur++;
	joueur->bufferAction = malloc(sizeof(char) * TAILLE_BUFFER);
	InfoJoueur inf;
	inf.pv = 100;
	inf.pvMax = 100;
	inf.exp = 0;
	/* La force aléatoire */
	//inf.degats = (rand() % 20) + 1;
	inf.degats = 5;
	runLog("degats du joueur : 5", 50);
	//runLogInt(inf.degats, 50);
	inf.nbTues = 0;
	joueur->info = inf;
	//init du mutex
	pthread_mutex_init(&joueur->mutex_Joueur, NULL);
	runLog("Joueur cree", 50);
	return joueur;
}

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




int digit_to_int(char d){
	char str[2];
	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}

int getSourceLongueur(char* mesg){
	//printf("int getSourceLongueur(char* mesg) \n");
	int nomSourceLongueur;
	nomSourceLongueur = 0;
	nomSourceLongueur = nomSourceLongueur + 10 * digit_to_int(mesg[1]);
	nomSourceLongueur = nomSourceLongueur + digit_to_int(mesg[2]);
	//printf("nomSourceLongueur : %i\n", nomSourceLongueur);
	return nomSourceLongueur;
}

int getCibleLongueur(char* mesg){
	//printf("int getCibleLongueur(char* mesg) \n");
	int nomCibleLongueur;
	nomCibleLongueur = 0;
	nomCibleLongueur = nomCibleLongueur + 10 * digit_to_int(mesg[3]);
	nomCibleLongueur = nomCibleLongueur + digit_to_int(mesg[4]);
	//printf("nomCibleLongueur : %i\n", nomCibleLongueur);
	return nomCibleLongueur;
}

int getDonneesLongueur(char* mesg){
	//printf("int getDonneesLongueur(char* mesg) \n");
	int donneesLongueur;
	donneesLongueur = 0;
	donneesLongueur = donneesLongueur + 100 * digit_to_int(mesg[6]);
	donneesLongueur = donneesLongueur + 10 * digit_to_int(mesg[7]);
	donneesLongueur = donneesLongueur + digit_to_int(mesg[8]);
	//printf("donneesLongueur : %i\n", donneesLongueur);
	return donneesLongueur;
}

int getPointsDeVie(char* mesg, int offset){
	//printf("int getPointsDeVie(char* mesg, int offset) \n");
	int pointsDeVie;
	pointsDeVie = 0;
	pointsDeVie = pointsDeVie + 100 * digit_to_int(mesg[offset]);
	pointsDeVie = pointsDeVie + 10 * digit_to_int(mesg[offset + 1]);
	pointsDeVie = pointsDeVie + digit_to_int(mesg[offset + 2]);
	//printf("pointsDeVie : %i\n", pointsDeVie);
	return pointsDeVie;
}

int getNbClient(char* mesg){
	//printf("int getNbClient(char* mesg) \n");
	int nbClient;
	nbClient = 0;
	nbClient = nbClient + 10 * digit_to_int(mesg[1]);
	nbClient = nbClient + digit_to_int(mesg[2]);
	//printf("nbClient : %i\n", nbClient);
	return nbClient;
}

int getLongueurNomClient(char* mesg, int offset){
	//printf("int getLongueurNomClient(char* mesg) \n");
	int longueurNomClient;
	longueurNomClient = 0;
	longueurNomClient = longueurNomClient + 10 * digit_to_int(mesg[offset]);
	longueurNomClient = longueurNomClient + digit_to_int(mesg[offset + 1]);
	//printf("longueurNomClient : %i\n", longueurNomClient);
	return longueurNomClient;
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

/** 
 * @brief génère les messages à envoyer
 */
char * genMessage(int port, char* host, char* nomSource, char* nomDest, int type, int tDm){
	//requete liste client
	char* message;
	if(type == 7){
		//boucle calculant la somme des noms 
		int i;
		int longueur = 0;
		for (i = 0; i < nbJoueursCourants; ++i) {
			longueur = longueur + strlen(joueurs[i]->nomJoueur);
		}
		message = calloc(1 + 2 * nbJoueursCourants + longueur, 1); 
		message = "5";
		for (i = 0; i < nbJoueursCourants; ++i) {
			if(strlen(joueurs[i]->nomJoueur) > 9){
				message = strcat(message, strlen(joueurs[i]->nomJoueur));
			}else{
				message = strcat(message, "0");
				message = strcat(message, strlen(joueurs[i]->nomJoueur));
			}
		}

		for (i = 0;i < nbJoueursCourants; ++i) {
			message = strcat(message, joueurs[i]->nomJoueur);
		}
	}
	//attaquer
	else if (type == 1) {
		int longueurNomDest = strlen(nomDest);
		int longueurNomSource = strlen(nomSource);
		message = calloc(1 + 2 + 2 + 3 + 1 + longueurNomDest + longueurNomSource + 3, 1);
		message = "3";
		//longueur des noms
		if(longueurNomDest > 9){
			message = strcat(message, longueurNomDest);
		}else{
			message = strcat(message, "0");
			message = strcat(message, longueurNomDest);
		}
		if(longueurNomSource > 9){
			message = strcat(message, longueurNomSource);
		}else{
			message = strcat(message, "0");
			message = strcat(message, longueurNomSource);
		}
		//concat
		message = strcat(message, "0");
		message = strcat(message, "003");
		message = strcat(message, nomSource);
		message = strcat(message, nomDest);
		message = strcat(message, "005");
	}
	//soigner
	else if (type == 2) {
			int longueurNomDest = strlen(nomDest);
		int longueurNomSource = strlen(nomSource);
		message = calloc(1 + 2 + 2 + 3 + 1 + longueurNomDest + longueurNomSource + 3, 1);
		message = "3";
		//longueur des noms
		if(longueurNomDest > 9){
			message = strcat(message, longueurNomDest);
		}else{
			message = strcat(message, "0");
			message = strcat(message, longueurNomDest);
		}
		if(longueurNomSource > 9){
			message = strcat(message, longueurNomSource);
		}else{
			message = strcat(message, "0");
			message = strcat(message, longueurNomSource);
		}
		//concat
		message = strcat(message, "1");
		message = strcat(message, "003");
		message = strcat(message, nomSource);
		message = strcat(message, nomDest);
		message = strcat(message, "005");
	}
	return message;
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
/* Fonction Principale */
int main(int argc, char** argv) {
	if (argc == 2) {
		//pour les calculs randoms de stats
		srand(time(NULL));

		pthread_t threadJeu;
		pthread_t threadJoueur[16];
		//TODO:faudrait check les arguments d'entree
		//gestion de signaux pour la terminaison du programme
		signal(SIGTERM, gestionSignal);
		signal(SIGINT, gestionSignal);
		//on commence par initialiser le "jeu"
		jeu = malloc(sizeof(Jeu));
		//initialisation du mutex
		pthread_mutex_init(&jeu->mutex_Jeu, NULL);
		initServer(atoi(argv[1]));
		int resultatJeu = pthread_create(&threadJeu, NULL, tourDeJeu, (void*) jeu);
		//si le thread de jeu echoue
		if (resultatJeu != 0) {
			runLog("Echec du thread de jeu", 0);
			runLogInt(resultatJeu, 0);
			return 1;			
		}
		int 
		socket_descriptor, /* descripteur de socket */
		new_socket_descriptor, /* [nouveau] descripteur de socket */
		longueur_adresse_courante; /* longueur d'adresse courante d'un client */
	
		sockaddr_in 
			adresse_locale, /* structure d'adresse locale*/
			adresse_client_courant; /* adresse client courant */
	
		hostent* ptr_hote; /* les infos recuperees sur la machine hote */
//		servent* ptr_service; /* les infos recuperees sur le service de la machine */

		/* recuperation du nom de la machine */
		/* recuperation de la structure d'adresse en utilisant le nom */
		if ((ptr_hote = gethostbyname(jeu->nomServeur)) == NULL) {
			perror("erreur : impossible de trouver le serveur a partir de son nom.");
			exit(1);
		}	
		/* initialisation de la structure adresse_locale avec les infos recuperees */
		/* copie de ptr_hote vers adresse_locale */
		bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
		adresse_locale.sin_family = ptr_hote->h_addrtype; 
		/* ou AF_INET */
		adresse_locale.sin_addr.s_addr = INADDR_ANY; 
		/* ou AF_INET */
		//on affecte le port qui a ete donne en commande

		adresse_locale.sin_port = htons(atoi(argv[1]));
		
		/*-----------------------------------------------------------*/
		printf("numero de port pour la connexion au serveur : %d \n", 
		ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
		/* creation de la socket */
		if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le client.");
			exit(1);
		}

		/* association du socket socket_descriptor a la structure d'adresse adresse_locale */
		if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
			perror("erreur : impossible de lier la socket a l'adresse de connexion.");
			exit(1);
		}
		
		/* initialisation de la file d'ecoute */
		listen(socket_descriptor,MAX_JOUEURS);
		

		/* attente des connexions et traitement des donnees recues */
		for(;;) {
			longueur_adresse_courante = sizeof(adresse_client_courant);
			/* adresse_client_courant sera renseignee par accept via les infos du connect */
			if ((new_socket_descriptor = accept(socket_descriptor, (sockaddr*) (&adresse_client_courant), &longueur_adresse_courante)) < 0) {
				perror("erreur : impossible d'accepter la connexion avec le client.");
				exit(1);
			}
			runLog("Tentative de connexion d'un joueur.", 0);

			/* on verifie que le nbJoueurs est pas max */
			if ( (nbJoueursCourants+1) == MAX_JOUEURS){
				runLog("Trop de joueurs connectes\n", 0);
				close(new_socket_descriptor);
			}
			
			/* Creation d'un joueur */
			Joueur* joueur = initJoueur(adresse_locale, new_socket_descriptor);
			//on assigne le nom du joueur sur le message
			char buffer[256];
			int longueur;
			if ((longueur = read(new_socket_descriptor, buffer, sizeof(buffer))) <= 0) {
				return 1;
			}
			/* évite les soucis de buffer */
			buffer[longueur] = '\0';
			printf("Connexion reçue.\n");
			runLog(buffer, 50);
			joueur->nomJoueur = buffer;
			ajoutJoueur(joueur);
			runLog(joueur->nomJoueur, 0);
			runLogInt(joueur->joueurId, 0);
			runLog("Joueur connecte", 0);
			pthread_create(&threadJoueur[joueur->joueurId], NULL, loopJoueur, (void *)joueur);
		}
		free(jeu);
	}
	else {
		perror("La bonne commande est ./csServ [port]");
	}
	return 0;
}