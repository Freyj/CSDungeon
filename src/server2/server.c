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
Ennemis* genEnnemis(Ennemis* en) {
	en->pvEn1 = 200;
	en->pvEn2 = 200;
	en->pvEn3 = 200;
	return en;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
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

void retraitJoueur(Joueur* joueur){
	int i;
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (joueurs[i]) {
			joueurs[i] = NULL;
			break;
		}
	}
  pthread_mutex_lock(&mutex);
  nbJoueursCourants--;
  pthread_mutex_unlock(&mutex);
}
/*------------------------------------------------------*/
/*-------------------PAS TESTE         -----------------*/
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
/*----------------// ENVOITOUS MARCHE PAS //------------*/
void gestionSignal(int nomSignal){
  int i;
  runLog("Signal reçu:", 50);
  runLog(strsignal(nomSignal), 50);
  /* Warn the clients that the server is closing */
  if (nomSignal == SIGINT || nomSignal == SIGTERM) {
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (joueurs[i]) {
			//envoiTous("Deconnexion Serveur.");
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
void ajoutJoueur(Joueur* joueur) {
	int i;
	for (i = 0; i < MAX_JOUEURS; i++) {
		if (!(&joueurs[i])) {
			joueurs[i] = joueur;
			break;
			}
		} 
		pthread_mutex_lock(&mutex);
		nbJoueursCourants++;
 		pthread_mutex_unlock(&mutex);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
Joueur* initJoueur(sockaddr_in adresse_locale, int nouv_sock) {
	Joueur* joueur = (Joueur *)calloc((sizeof(Joueur)), 1);
	joueur->nomJoueur = "N00b";
	joueur->adresse_locale = adresse_locale;
	joueur->nouv_sock = nouv_sock;
	joueur->joueurId = idLibre();
	joueur->bufferAction = malloc(sizeof(char) * TAILLE_BUFFER);
	joueur->tourEnCours = 0;
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
	runLog("Joueur cree", 50);
	//char* mess = strcat("Un nouveau joueur arrive : ", (char*) joueur->nomJoueur);
	//runLog(mess, 0);
	pthread_mutex_lock(&mutex);
	joueurIdCompteur++;
	pthread_mutex_unlock(&mutex);
	return joueur;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int digit_to_int(char d){
	char str[2];
	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
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
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getSourceLongueur(char* mesg){
	return getDonnees(mesg, 2, 1, 2, 0);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getCibleLongueur(char* mesg){
	return getDonnees(mesg, 2, 3, 4, 0);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getDonneesLongueur(char* mesg){
	return getDonnees(mesg, 3, 6, 7, 8);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getPointsDeVie(char* mesg, int offset){
	return getDonnees(mesg, 3, offset, offset + 1, offset + 2);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getNbClient(char* mesg){
	return getDonnees(mesg, 2, 1, 2, 0);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int getLongueurNomClient(char* mesg, int offset){
	return getDonnees(mesg, 2, offset, offset + 1, 0);
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
	longueurEntete = 9;
	printf("\t\t\n");
	printf("%s\n", mesg);
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
		//ACTION
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
/*------------------------------------------------------*/
/*------------------------------------------------------*/

char* genMessage(int port, char* host, char* nomSource, char* nomDest, int type, int tDm){
	//requete liste client
	char* message;
	if(type == 7){
		//boucle calculant la somme des noms 
		int i;
		int longueur = 0;
		for (i = 0; i < nbJoueursCourants; ++i) {
			longueur = longueur + strlen((char*) joueurs[i]->nomJoueur);
		}
		message = calloc(1 + 2 * nbJoueursCourants + longueur, 1); 
		message = "5";
		for (i = 0; i < nbJoueursCourants; ++i) {
			if(strlen(joueurs[i]->nomJoueur) > 9){
				message = strcat(message, (char*) strlen((char*) joueurs[i]->nomJoueur));
			}else{
				message = strcat(message, "0");
				message = strcat(message, (char*) strlen((char*) joueurs[i]->nomJoueur));
			}
		}

		for (i = 0;i < nbJoueursCourants; ++i) {
			message = strcat(message, joueurs[i]->nomJoueur);
		}
	}
	//deconnexion
	else if (type == 6) {

	}
	//attaquer
	else if (type == 1) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3, 1);
		message = "3";
		//longueur des noms
		if(strlen(nomDest) > 9){
			message = strcat(message, (char*) strlen(nomDest));
		}else{
			message = strcat(message, "0");
			message = strcat(message, (char*) strlen(nomDest));
		}
		if(strlen(nomSource) > 9){
			message = strcat(message, (char*) strlen(nomSource));
		}else{
			message = strcat(message, "0");
			message = strcat(message, (char*) strlen(nomSource));
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
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3, 1);
		message = "3";
		//longueur des noms
		if(strlen(nomDest) > 9){
			message = strcat(message, (char*) strlen(nomDest));
		}else{
			message = strcat(message, "0");
			message = strcat(message, (char*) strlen(nomDest));
		}
		if(strlen(nomSource) > 9){
			message = strcat(message, (char*) strlen(nomSource));
		}else{
			message = strcat(message, "0");
			message = strcat(message, (char*) strlen(nomSource));
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
int isNotJoueur(int sock_desc) {
	int i;
	for (i = 0; i < nbJoueursCourants; ++i) {
		if (joueurs[i]) {
			if (joueurs[i]->nouv_sock == sock_desc) {
				return 0;
			}
		}
	}
	return 1;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int idLibre() {
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (!joueurs[i]) {
			return i;
		}
	}
	return -1;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int idOccupe() {
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (joueurs[i]) {
			return i;
		}
	}
	return -1;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
void* loopJoueur(void* arg) {
	int longueur;
	char buffer[TAILLE_BUFFER];
	Joueur* joueur = (Joueur*) arg;

	for (;;) {
		if (joueur->tourEnCours == 1) {
			if ((longueur = read(joueur->nouv_sock, buffer, TAILLE_BUFFER)) > 0) {
				/* évite les soucis de buffer */
				buffer[longueur] = '\0';
				printf("Message joueur en cours reçu.\n");
				runLog(buffer, 50);
				pthread_mutex_lock(&mutex);
				joueur->bufferAction = buffer;
				//envoiTous(resolution(joueur));
				//une fois qu'on a lu une réponse, c'est plus le tour du joueur
				joueur->tourEnCours = 0;
				//on passe le token
				joueurs[joueur->joueurId]->tourEnCours = 1;
				pthread_mutex_lock(&mutex);
	       }
	       //sinon, on fait rien (idéalement on devrait faire dormir)
	       //ou mieux, avoir une condition de reveil du thread
	    //sinon, c'est que y a eu une déco impromptue
	    else {
       	//	retraitJoueur(joueur);
       		char* messageDeco = "deco du client";
       		runLog(messageDeco, 2);
       	//	envoiTous(messageDeco);
       		//free(joueur);
       		return NULL;
	    }
	    //petite pause pour pas spammer
	    sleep(5);
	   }
	}
	pthread_detach(pthread_self());
	return NULL;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
char* resolution(Joueur* joueur) {
	return NULL;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
void* loopJeu(void* arg) {
	//boucle infinie pour tourner
	for (;;) {
		//si le jeu est pas encore commencé, on lance le jeu
		//le tour 0 ne compte pas
		if ((nbJoueursCourants > 1) && (jeu->nbTour == 0) ) {
			pthread_mutex_lock(&mutex);
			joueurs[idOccupe()]->tourEnCours = 1;
			pthread_mutex_lock(&mutex);
			pthread_mutex_lock(&mutex);
			jeu->nbTour++;
			pthread_mutex_lock(&mutex);			
		}
		else {
			printf("pas assez de joueurs\n");
		}
		sleep(5);
	}
	pthread_detach(pthread_self());
	return NULL;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
void initServer(int port) {
	//comme on modifie le jeu
	runLog("debut d'initialisation", 0);
	//theoriquement, le mutex n'est pas necessaire ici, mais au cas où
	pthread_mutex_lock(&mutex);
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

	pthread_mutex_unlock(&mutex);
	runLog("initialisation terminee", 0);
}


/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/*------------------------------------------------------*/
int main(int argc, char** argv) {
	if ( argc == 2 ) {
		//gestion de signaux pour la terminaison du programme
		pthread_t threadJeu;
		signal(SIGTERM, gestionSignal);
		signal(SIGINT, gestionSignal);
		//on commence par initialiser le "jeu"
		jeu = malloc(sizeof(Jeu));
		initServer(atoi(argv[1]));
		int new_socket_descriptor,  /* new socket descriptor */
	    address_length; /* client address length */
		sockaddr_in local_address,    /* local address socket informations */
	    cli_addr;  /* client address */
		hostent* ptr_host;  /* informations about host */
		char host_name[TAILLE_MAX_NOM]; /* host name */

		pthread_create(&threadJeu, NULL, loopJeu, NULL);
		gethostname(host_name,TAILLE_MAX_NOM);  /* getting host name */

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
		local_address.sin_port = htons(atoi(argv[1]));
		printf("Using port : %d \n", ntohs(local_address.sin_port));
		/* create socket in socket_descriptor */
		if ((descripteurSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("error: unable to create the connection socket.");
			exit(1);
		  }
		  /* bind socket descripteurSocket to sockaddr_in local_address */
		if ((bind(descripteurSocket, (sockaddr*)(&local_address), sizeof(local_address))) < 0) {
			perror("error: unable to bind the socket to the connection address.");
			exit(1);
		}
	  	/* initialize the queue */
	  	listen(descripteurSocket,MAX_JOUEURS);

		for(;;) {
			address_length = sizeof(cli_addr);
			//on cherche à accepter la connexion
			if ((new_socket_descriptor = accept(descripteurSocket, (sockaddr*) (&cli_addr), (socklen_t*) &address_length)) < 0) {
				perror("erreur : impossible d'accepter la connexion avec le client.");
				exit(1);
			}
			printf("%s\n", "TESTING NB JOUEURS");
			printf("%d\n", nbJoueursCourants );
			if ((nbJoueursCourants+1) == MAX_JOUEURS){
				char* messageErr = "64565"; 
				if (write(new_socket_descriptor, messageErr, strlen(messageErr)+1) == -1) {
					runLog("erreur d'envoi vers client excedentaire", 0);
				}
				else {
					runLog("rejet de joueur en trop bien envoye", 0);
				}
				close(new_socket_descriptor);
			}
			else {
				runLog("Tentative de connexion d'un joueur.", 0);
				/* Creation d'un joueur */
				Joueur* joueur = initJoueur(cli_addr, new_socket_descriptor);
				ajoutJoueur(joueur);
				//on assigne le nom du joueur sur le message
				pthread_create(&threadJoueur[joueur->joueurId], NULL, loopJoueur, (void *)joueur);
				printf("Joueur cree\n");
				//char* testGen = genMessage()
				char* messCrea = "706000000ELDRAD";
				if (write(joueur->nouv_sock, messCrea, strlen(messCrea)+1) == -1) {
				runLog("erreur d'envoi vers client normal", 0);
				}
				else {
					runLog("message envoyé au client", 0);
				}
			}
		}
		//liberer le jeu
		free(jeu);
	}
	else {
		perror("La bonne commande est ./csServ [port]");
	}
	return 0;

}