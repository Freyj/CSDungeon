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
void initJoueur(int sock, char* nom) {
		Joueur* joueur = malloc(sizeof(Joueur));
		
		joueur->nomJoueur = nom;
		joueur->sock_desc = sock;
		joueur->pv = 100;
		joueur->degats = 5;
		joueur->joueurId = nbJoueursCourants;
		joueurs[nbJoueursCourants] = joueur;

		nbJoueursCourants++;
}

/*------------------------------------------------------*/
/*-------------------PAS TESTE         -----------------*/
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
/*-------------------PAS TESTE         -----------------*/
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
/*-------------------PAS TESTE         -----------------*/
char* action(char* buffer) {
	char* res = "00000000000";
	return res;
}


int main(int argc, char** argv) {
	if (argc != 2) {
		perror("La bonne commande est ./csServ [port]");
	}
	else {
		jeu = malloc(sizeof(Jeu));
		initServer(atoi(argv[1]));
		int new_socket_descriptor,  /* new socket descriptor */
	    address_length; /* client address length */
		sockaddr_in local_address,    /* local address socket informations */
	    cli_addr;  /* client address */
		hostent* ptr_host;  /* informations about host */
		char host_name[TAILLE_MAX_NOM]; /* host name */

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

		printf("CA BLOQUE\n");
		  /* bind socket descripteurSocket to sockaddr_in local_address */
		if ((bind(descripteurSocket, (sockaddr*)(&local_address), sizeof(local_address))) < 0) {
			perror("error: unable to bind the socket to the connection address.");
			exit(1);
		}
	  	/* initialize the queue */
	  	listen(descripteurSocket,MAX_JOUEURS);

		printf("Server initialise\n");

	  	//on attend la connexion de tous les joueurs
	  	while(nbJoueursCourants < MAX_JOUEURS) {
	  		printf("ajouts de joueurs\n");
			address_length = sizeof(cli_addr);
			if ((new_socket_descriptor = accept(descripteurSocket, (sockaddr*) (&cli_addr), (socklen_t*) &address_length)) < 0) {
				perror("erreur : impossible d'accepter la connexion avec le client.");
				exit(1);
			}

			//initialise le joueur ET incrémente le nbJoueursCourant. (ajoute dans le tableau)
			//how to get a name ?
			initJoueur(new_socket_descriptor, "Bobby");
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
						printf("joueur %s\n", joueurs[iterJoueur]->nomJoueur);
						int longueur = 0;
						char* buffer = "test";						
						//on écoute
						if ((longueur = read(joueurs[iterJoueur]->sock_desc, buffer, TAILLE_BUFFER)) > 0) {
							printf("%s\n", buffer);
							//si le joueur n'est pas mort
							if (joueurs[iterJoueur]->pv > 0) {
								printf("%i\n", joueurs[iterJoueur]->pv);
								//si les ennemis sont morts, on envoie un message de fin
								//TODO: plus tard, on enverra un truc pour redémarrer?
								if (ennemisElimines(jeu->ennemis) == 1) {
									//message de fin (le 9 envoie fin de jeu, le cinquième caractere determine vitoire
									//ou défaite (1 ou 0))
									char* bufferFinJeu = "90001";
									envoiTous(bufferFinJeu);
									printf("victoire des joueurs\n");
									return 0;
								}
								else {
									buffer[longueur] = '\0';
									runLog(buffer, 0);
									joueurs[iterJoueur]->bufferAction = buffer;
									char* bufferRenvoi;
									//on traite l'action liée au buffer
									bufferRenvoi = action(buffer);
									//on broadcast à tout le monde l'info
									envoiTous(bufferRenvoi);
								}
							}
							//on ignore les joueurs morts
						}
					}
					//le joueur existe pas on passe au suivant (pas opti)
				}
			}
			else {
				//si ils sont tous morts, on envoi un message de défaite
				//message de fin (le 9 envoie fin de jeu, le cinquième caractere determine vitoire
				//ou défaite (1 ou 0))
				char* bufferFinJeu = "90000";
				envoiTous(bufferFinJeu);
				printf("defaite des joueurs\n");
				return 0;
			}
		}
	}
	return 0;
}