/**
 * @Authors : Charlene Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.2
 * @brief : 
*/
/*----------------------------------------------
Serveur a lancer avant le client

si y a pas moyen de mettre un booléen dans le client genre "jai_la_parole"
dans le client pardon je veux dire dans le thread client du serveur


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
#define TAILLE_MAX_NOM 13
#define MAX_JOUEURS 3
#define TAILLE_BUFFER 256


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


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
pthread_t threadJoueur[MAX_JOUEURS];




/**
 * @brief Fonction qui donne l'heure pour les logs
 * @details renvoie l'heure en hh:mm:ss
 * @return un char* decrivant l'heure
 */
char* heure();
/**
 * @brief Fonction pour logger les erreurs dans un fichier
 * @param erreur le message en char*
 * @param threadNb le nb du thread pour debug 
 */
void runLog(char* erreur, int threadNb);
/**
 * @brief Fonction pour logger les erreurs dans un fichier
 * fonctionne pour les int
 * @param erreur le message en int
 * @param threadNb le nb du thread pour debug 
 */
void runLogInt(int erreur, int threadNb);


/**
 * @brief Fonction generant un groupe d'ennemis
 * @details 3 ennemis a 20 pv, pas encore de reglages
 */
Ennemis* genEnnemis(Ennemis* en);
/**
 * @brief fonction verifiant qu'un groupe n'est pas mort
 */
int ennemisElimines(Ennemis* en);
/**
 * @brief fonction lançant une attaque sur un groupe
 */
void attaque(int degats);
/** @brief fonction qui enleve un joueur et decremente le total
 *
 */
void retraitJoueur(Joueur* joueur);
/**
 * @brief envoie un message a tous les joueurs
 * @details log les envois
 * @param message le char* du message
 */
void envoiTous(char* message);
/**
 * @brief gere les signaux 
 * @param nomSignal : l'int du signal (SIGINT)
 */
void gestionSignal(int nomSignal);
/** 
 * @brief fonction qui ajoute un joueur et incremente le total
 */
void ajoutJoueur(Joueur* joueur);
/**
 * @brief  Initialisation du joueur
 */
Joueur* initJoueur(sockaddr_in adresse_locale, int nouv_sock);
/**
 *	@brief initialisation du serveur
 */ 
void initServer(int port);
int digit_to_int(char d);
int getSourceLongueur(char* mesg);
int getCibleLongueur(char* mesg);
int getDonneesLongueur(char* mesg);
int getPointsDeVie(char* mesg, int offset);
int getNbClient(char* mesg);
int getLongueurNomClient(char* mesg, int offset);
void decode(char* mesg);
/** 
 * @brief génère les messages à envoyer
 */
char * genMessage (int port, char* host, char* nomSource, char* nomDest, int type, int tDm);
/**
 * @brief vérifie que le socket actuel n'existe pas déjà
 * dans la liste de joueurs
 * @param le socket qu'on compare
 * @return un int, 0 si c'est le même
 *                 1 sinon
 */
int isNotJoueur(int sock_desc);

/** 
 * @brief fonction du thread de chaque joueur
 */
void* loopJoueur(void* arg);

/** 
 * @brief fonction du thread de jeu
 */
void* loopJeu(void* arg);

/** 
 * @brief fonction de resolution d'action d'un joueur
 * @param joueur agissant
 * @return message à renvoyer à tous
 */
char* resolution(Joueur* joueur);
/**
 * @brief renvoie le premier id non attribue
 * @param l'int a testé
 * @return l'int non attribue, sinon -1
 */
int idLibre();
/**
 * @brief renvoie le premier id attribue
 * @param l'int a testé
 * @return l'int attribue, sinon -1
 */

int idOccupe();