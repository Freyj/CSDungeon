/**
 * @Authors : Charlene Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.2
 * @brief : 
*/
/*----------------------------------------------
Serveur a lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> 
/* pour les sockets */
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h> 
/* pour hostent, servent */
#include <string.h> 
/* pour bcopy, ... */  
#include <unistd.h>
/* pour recuperer le temps*/ 
#include <time.h> 
#include <pthread.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
/* pour les signaux */
#include <signal.h>

#define TAILLE_MAX_NOM 256
#define MAX_JOUEURS 16

// pour compter les joueurs
static int nbJoueursCourants = 0;
//pour les identifier
static int joueurIdCompteur = 0;
//int du socket
static int descripteurSocket; /* socket descriptor */

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
	int force;
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
	pthread_mutex_t mutex_Jeu;
	int port;
	char* nomServeur;
} Jeu;

//joueurs et jeu variables globales
Joueur* joueurs[MAX_JOUEURS];
Jeu* jeu;


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
	en->pvEn1 = 20;
	en->pvEn2 = 20;
	en->pvEn3 = 20;
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
void attaque(Ennemis* en, int degats) {
	if (ennemisElimines(en) == 0) {
		if (en->pvEn1 > 0 ) {
			en->pvEn1 = en->pvEn1 - degats;
			runLog("ennemi 1 tape", 50);
		}
		else if(en->pvEn2 > 0 ) {
			en->pvEn2 = en->pvEn2 - degats;
			runLog("ennemi 2 tape", 50);
		}
		else if(en->pvEn3 > 0 ) {
			en->pvEn3 = en->pvEn3 - degats;
			runLog("ennemi 3 tape", 50);
		}
	}
	runLog("ennemis tues", 50);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction qui soigne
 * TODO: do
 */
void soigner() {
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/** 
 * Fonction qui gere le tour
 */ 
void* tourDeJeu(void* arg) {
	Jeu* jeu = (Jeu*) arg;
	pthread_mutex_lock(&jeu->mutex_Jeu);
	pthread_mutex_unlock(&jeu->mutex_Jeu);
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
	pthread_mutex_init(&jeu->mutex_Jeu, NULL);
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
  if (nomSignal == SIGINT) {
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
void* loop_joueur(void* arg){
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
	InfoJoueur inf;
	inf.pv = 100;
	inf.pvMax = 100;
	inf.exp = 0;
	/* La force est comprise entre 1 et 20 */
	inf.force = (rand() % 20) + 1;
	runLog("Force du joueur :", 50);
	runLogInt(inf.force, 50);
	inf.nbTues = 0;
	joueur->info = inf;
	runLog("Joueur cree", 50);
	return joueur;
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
/* Fonction Principale */
int main(int argc, char** argv) {
	if (argc == 2) {
		//pour les calculs randoms de stats
		srand(time(NULL));

		pthread_t threadJeu;
		pthread_t threadJoueur;
		//TODO:faudrait check les arguments d'entree
		//gestion de signaux pour la terminaison du programme
		signal(SIGTERM, gestionSignal);
		signal(SIGINT, gestionSignal);
		//on commence par initialiser le "jeu"
		jeu = malloc(sizeof(Jeu));
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
			runLog(buffer, 50);
			joueur->nomJoueur = buffer;
			runLog(joueur->nomJoueur, 0);
			runLogInt(joueur->joueurId, 0);
			runLog("Joueur connecte", 0);
			ajoutJoueur(joueur);
			pthread_create(&threadJoueur, NULL, loop_joueur, (void *)joueur);
		}
		free(jeu);
	}
	else {
		perror("La bonne commande est ./csServ [port]");
	}
	return 0;
}