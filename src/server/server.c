/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.2
 * @brief : 
*/

/*----------------------------------------------
Serveur à lancer avant le client
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
/* pour récupérer le temps*/ 
#include <time.h> 
#include <pthread.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>

#define TAILLE_MAX_NOM 256
#define MAX_JOUEURS 16

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

/* Structure pour les armes 
typedef struct arme {
	char* nom;
	int estMagique;
	int degats;
	int precision;
}arme;
*/
/* Structure pour stocker les infos d'un personnage */
typedef struct _Infoclient {
	/* nom  */
	char* nom;
	/* caractéristiques */
	int pv;
	int pvMax;
	int exp;
	//int niveau;
	int force;
	//int magie;
	//int technique;
	//int vitesse;
	//int chance;
	//int defense;
	//int resistance;
	//arme arme;
	/*socket associé */
}Infoclient;

typedef struct _Client {
	Infoclient info;
	int sock;
	char* host;
}Client;

// typedef struct _DonneesThread{
//  // Server *server;
//   int sock_send;
//   int sock_read;
//   int status;
//   char* buffer_send;
//   char* buffer_read;
//   pthread_t thread_read;
//   pthread_t  thread_send;
//   pthread_mutex_t mutex_data;
// } DonneesThread;

typedef struct _Ennemis{
	int pvEn1;
	int pvEn2;
	int pvEn3;
} Ennemis;

typedef struct _Jeu{
	Ennemis* ennemis;
	Client* clients;
	int nbClients;
	int nbTour;
	pthread_t* threadsClients;
	pthread_t* threadJeu;
	pthread_t* threadEcoute; 
	pthread_mutex_t mutex_Jeu;
} Jeu;
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief Fonction qui donne l'heure pour les logs
 * @details renvoie l'heure en hh:mm:ss
 * @return un char* décrivant l'heure
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
 * @brief Fonction générant un groupe d'ennemis
 * @details 3 ennemis à 20 pv, pas encore de réglages
 */
Ennemis* genEnnemis(Ennemis* en){
	en->pvEn1 = 20;
	en->pvEn2 = 20;
	en->pvEn3 = 20;
	return en;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief fonction vérifiant qu'un groupe n'est pas mort
 */

int ennemisElimines(Ennemis* en) {
	runLog("Check d'ennemis éliminés", 50);
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
			runLog("ennemi 1 tapé", 50);
		}
		else if(en->pvEn2 > 0 ) {
			en->pvEn2 = en->pvEn2 - degats;
			runLog("ennemi 2 tapé", 50);
		}
		else if(en->pvEn3 > 0 ) {
			en->pvEn3 = en->pvEn3 - degats;
			runLog("ennemi 3 tapé", 50);
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
 * Fonction qui gère le tour
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
 * @brief Fonction réagissant à l'action d'un client
 */ 
void action(int sock, Ennemis* en) {
	char buffer[256];
	int longueur;
	if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) {
		return;
	}
	//on prévient le client qu'il a fait quelque chose
	//char* reponse;

	// if (strcmp(buffer, "Attaque") == 0) {
	// 	//attaque(en, 2);
	// 	reponse = "Vous avez attaqué";
	// 	write(sock,reponse,strlen(reponse)+1);   
	// }
	// if (strcmp(buffer, "Soigner") == 0) {
	// 	soigner();
	// 	reponse = "Vous vous êtes soigné";
	// 	write(sock,reponse,strlen(reponse)+1);   
	// }
	//0 est un int, et équivalent à \0
	memset(&buffer, 0, 1);
}
/**
 *  @brief initialisation du serveur
 */ 
void initServer(Jeu* jeu) {
	//comme on modifie le jeu
	runLog("début d'initialisation", 0);
	//théoriquement, le mutex n'est pas nécessaire ici, mais au cas où
    pthread_mutex_lock(&jeu->mutex_Jeu);
    //initialisation des adversaires
    Ennemis* groupeEnnemis = malloc(sizeof(Ennemis));
	groupeEnnemis = genEnnemis(groupeEnnemis);
	jeu->ennemis = groupeEnnemis;
	runLog("Pvs de l'ennemi 1 pour test", 0);
	runLogInt(jeu->ennemis->pvEn1, 0);
	//compteur de tours
	jeu->nbTour = 0;
	//initialisation d'un tableau de clients
	jeu->clients = NULL;//?
	jeu->nbClients = 0;
	//init threads
	jeu->threadsClients = malloc(sizeof(pthread_t));
	jeu->threadJeu = malloc(sizeof(pthread_t));
	jeu->threadEcoute = malloc(sizeof(pthread_t));

	pthread_mutex_unlock(&jeu->mutex_Jeu);
	runLog("initialisation terminée", 0);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
/**
 * @brief broadcast les infos du tour aux clients
 *	cf protocole
 * @details log les envois
 */
void broadcast(int port, Client* clients, char* message){
	int i;
	for (i = 0; i < MAX_JOUEURS; ++i) {
		if (write(clients[i].sock,message,strlen(message)+1) == -1) {
			runLog("Erreur d'envoi pour le client", 50);
			runLog(clients[i].info.nom, 50);
		}	
		else {
			runLog("Envoi correct pour le client", 50);
			runLog(clients[i].info.nom, 50);
		}
	}
}

/**
 *  @brief fonction d'écoute
 */

void* ecoute(void* arg){
	//Jeu jeu = (Jeu) arg;
	runLog("I am listening!", 1);


	return NULL;
}


int main(int argc, char** argv) {
	if (argc == 2) {
		//TODO:faudrait check les arguments d'entrée
		//on commence par initialiser le "jeu"
		Jeu* jeu;
		jeu = malloc(sizeof(Jeu));
		initServer(jeu);
		//une fois que le jeu est initialisé, on lance l'écoute 
		int resultatEcoute = pthread_create(jeu->threadEcoute, NULL, ecoute, (void*) jeu);
		//si le thread d'écoute échoue
		if (resultatEcoute != 0) {
			runLog("Echec du thread d'écoute", 0);
			runLogInt(resultatEcoute, 0);
			return 1;
		}
		//puis on lance le "jeu" 
		int resultatJeu = pthread_create(jeu->threadJeu, NULL, tourDeJeu, (void*) jeu);
		//si le thread de jeu échoue
		if (resultatJeu != 0) {
			runLog("Echec du thread de jeu", 0);
			runLogInt(resultatJeu, 0);
			return 1;			
		}


		int 
		socket_descriptor, /* descripteur de socket */
		nouv_socket_descriptor, /* [nouveau] descripteur de socket */
		longueur_adresse_courante; /* longueur d'adresse courante d'un client */
	
		sockaddr_in 
			adresse_locale, /* structure d'adresse locale*/
			adresse_client_courant; /* adresse client courant */
	
		hostent* ptr_hote; /* les infos recuperees sur la machine hote */
//		servent* ptr_service; /* les infos recuperees sur le service de la machine */
		char machine[TAILLE_MAX_NOM+1]; 

		/* nom de la machine locale */
		gethostname(machine,TAILLE_MAX_NOM);
		/* recuperation du nom de la machine */
		/* recuperation de la structure d'adresse en utilisant le nom */
		if ((ptr_hote = gethostbyname(machine)) == NULL) {
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
		//on affecte le port qui a été donné en commande

		adresse_locale.sin_port = htons(atoi(argv[1]));
		
		/*-----------------------------------------------------------*/
		printf("numero de port pour la connexion au serveur : %d \n", 
		ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
		/* creation de la socket */
		if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("erreur : impossible de creer la socket de connexion avec le client.");
			exit(1);
		}

		/* association du socket socket_descriptor à la structure d'adresse adresse_locale */
		if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
			perror("erreur : impossible de lier la socket a l'adresse de connexion.");
			exit(1);
		}
		
		/* initialisation de la file d'ecoute */
		listen(socket_descriptor,5);
		
		/* Thread d'écoute du serveur */

		/* attente des connexions et traitement des donnees recues */
		for(;;) {
			longueur_adresse_courante = sizeof(adresse_client_courant);
			/* adresse_client_courant sera renseignée par accept via les infos du connect */
			if ((nouv_socket_descriptor =  accept(socket_descriptor, (sockaddr*) (&adresse_client_courant), &longueur_adresse_courante)) < 0) {
				perror("erreur : impossible d'accepter la connexion avec le client.");
				exit(1);
			}
			/* traitement du message */
			printf("reception d'un message.\n");
			int i = 0;
				while (i < 2000){
				//action(nouv_socket_descriptor, groupeEnnemis);
//				renvoi(nouv_socket_descriptor);
				close(nouv_socket_descriptor);
				++i;
			}
		}






		free(jeu);
	}
	else {
		perror("La bonne commande est ./csServ [port]");
	}
	return 0;
}