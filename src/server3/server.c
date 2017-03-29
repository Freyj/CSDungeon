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
/*-------------------PAS FINI---------------------------*/
char* action(char* buffer, Joueur* joueurCourant) {
	char* res;
	int type = getTypeMessage(buffer);
	//un client se co
	if (type == 0) {
		res = calloc(10, 1);
		res = "000000000";
		printf("Une connexion en cours de jeu, souci.\n");
	}
	//un client attaque
	else if (type == 1) {
		printf("%s\n", "attaque du client" );
	}
	//un client se soigne
	else if (type == 2) {
		printf("%s\n", "soin du client" );
	}
	//un client deco
	else if (type == 6) {
		joueurCourant->pv = 0;
		res = genMessage(joueurCourant->nomJoueur, "ALL", 6, 0);
		printf("%s\n", "une deconnexion");
	}
	//on demande la liste des clients
	else if (type == 7){
		printf("%s\n", "une liste de clients demandée");

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

char* genMessage(char* nomSource, char* nomDest, int type, int tDm){
	//requete liste client
	char* message;
	if(type == 7){
		//boucle calculant la somme des noms 
		int i;
		int longueur = 0;
		for (i = 0; i < nbJoueursCourants; ++i) {
			longueur = longueur + strlen((char*) joueurs[i]->nomJoueur);
		}
		printf("Taille\n");
		printf("%i\n", longueur);
		message = calloc(longueur, 1);
		message = strcpy(message, "5 \0");
		for (i = 0; i < nbJoueursCourants; ++i) {
			printf("i : %i\n", i);
			if(strlen(joueurs[i]->nomJoueur) > 9){
				printf("%s\n", message);
				char* nom = strcat(joueurs[i]->nomJoueur, "\0");
				message = strcat(message, "bob\0");
				printf("%s\n", message);

				message = strcat(message, (char*) strlen("10"));
			}else{
				message = strncat(message, "0\0", 1);
				message = strncat(message, (char*) strlen((char*) joueurs[i]->nomJoueur), 1);
			}
		}

		for (i = 0;i < nbJoueursCourants; ++i) {
			message = strcat(message, joueurs[i]->nomJoueur);
		}

		message = strcat(message, "\0");
	}
	//deconnexion
	else if (type == 6) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		message = "6";	
		//longueur des noms
		if(strlen(nomDest) > 9){
			message = strncat(message, (char*) strlen(nomDest), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomDest), 1);
		}
		if(strlen(nomSource) > 9){
			message = strncat(message, (char*) strlen(nomSource), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomSource), 1);
		}
		message = strcat(message, "\0");
	}
	//attaquer
	else if (type == 1) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		message = "3";
		//longueur des noms
		if(strlen(nomDest) > 9){
			message = strncat(message, (char*) strlen(nomDest), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomDest), 1);
		}
		if(strlen(nomSource) > 9){
			message = strncat(message, (char*) strlen(nomSource), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomSource), 1);;
		}
		//concat
		message = strncat(message, "0", 1);
		message = strncat(message, "003", 3);
		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, nomDest, strlen(nomDest));
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);
	}
	//soigner
	else if (type == 2) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomDest) + strlen(nomSource) + 3+1, 1);
		message = "3";
		//longueur des noms
		if(strlen(nomDest) > 9){
			message = strncat(message, (char*) strlen(nomDest), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomDest), 1);
		}
		if(strlen(nomSource) > 9){
			message = strncat(message, (char*) strlen(nomSource), 2);
		}else{
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomSource), 1);
		}
		//concat
		message = strncat(message, "1", 1);
		message = strncat(message, "003", 3);
		message = strncat(message, nomSource, strlen(nomSource));
		message = strncat(message, nomDest, strlen(nomDest));
		message = strncat(message, "005", 3);
		message = strncat(message, "\0", 1);

	}
	else if (type == 8) {
		message = calloc(1 + 2 + 2 + 3 + 1 + strlen(nomSource)+10, 1);
		message = "8";
		if(strlen(nomSource) > 9){
			message = strncat(message, (char*) strlen(nomSource), 2);
		}else{
			printf("test\n");
			printf("%s\n", message);
			printf("%d\n", 1 + 2 + 2 + 3 + 1 + strlen(nomSource)+1);
			message = strncat(message, "0", 1);
			message = strncat(message, (char*) strlen(nomSource), 1);
		}
		message = strncat(message, "00", 2);
		message = strncat(message, "0", 1);
		message = strncat(message, "000", 3);
		message = strncat(message, nomDest, strlen(nomDest));
		message = strncat(message, "\0", 1);
	}
	return message;
}


/*------------------------------------------------------*/
/*------------------------------------------------------*/



























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
				printf("%s\n", "AHHHHHHHHHHHHHHHHHHH");
			}

			//initialise le joueur ET incrémente le nbJoueursCourant. (ajoute dans le tableau)
			//init de nom pourri pour test
			initJoueur(new_socket_descriptor, "Bobby123456789", adresse_client_courant);
		}

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
						int longueur = 0;
						char buffer[TAILLE_BUFFER];
						//on écoute
						//segfault
						//printf("%d\n", joueurs[iterJoueur]->sock_desc);
						setsockopt(joueurs[iterJoueur]->sock_desc, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&timeoutRead,sizeof(struct timeval));
						//envoi du token au joueur en question
						//generation du message, de type 8 (token), 0 parce qu'on s'en fiche
						//50 = max size (large)
						char* msg = calloc(TAILLE_BUFFER, sizeof(char));
						msg = genMessage(joueurs[iterJoueur]->nomJoueur,joueurs[iterJoueur]->nomJoueur, 8, 0);
						printf("(%s\n", "BOUH");
						printf("%s\n", msg);
						envoiTous(msg);
						longueur = read(joueurs[iterJoueur]->sock_desc, buffer, TAILLE_BUFFER-1);
						//runLogInt(errno,1);
						//printf("%s\n", strerror(errno));
						if (longueur < 0) {
							printf("ERROR DE READ\n");
							joueurs[iterJoueur] = NULL;
						}
						else if (longueur == 0) {
							printf("CLOSE DE SOCKET\n");
							joueurs[iterJoueur] = NULL;							
						}
						if (longueur > 0) {
							//printf("%s\n", buffer);
							//si le joueur n'est pas mort
							printf("RECEIVED A THING\n");
							if (joueurs[iterJoueur]->pv > 0) {
								printf("JOUEUR ALIVE\n");
								printf("PV du joueur %i\n", joueurs[iterJoueur]->pv);
								//si les ennemis sont morts, on envoie un message de fin
								//TODO: plus tard, on enverra un truc pour redémarrer?
								if (ennemisElimines(jeu->ennemis) == 1) {
									printf("VICTOIRE\n");
									//message de fin (le 9 envoie fin de jeu, le cinquième caractere determine vitoire
									//ou défaite (1 ou 0))
									char* bufferFinJeu = "900001";
									envoiTous(bufferFinJeu);
									printf("victoire des joueurs\n");
									return 0;
								}
								else {
									printf("SOMETHING HAPPENED\n");
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
						printf("DID NOT READ\n");
					}
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