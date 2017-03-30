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
/* pour les erreurs */
#include <errno.h> 

#define TAILLE_MAX_NOM 13
#define MAX_JOUEURS 4
#define MAX_TOURS 50
#define TAILLE_BUFFER 1024
#define READ_TIMEOUT 3


// pour compter les joueurs
static int nbJoueursCourants = 0;

//structures pour les sockets
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;
typedef struct timeval timeval;


/* Structure pour stocker les infos d'un client*/
typedef struct _Joueur {
	int sock_desc;
	int joueurId;
	int longueur_adresse_courante;
	struct sockaddr_in adresse_locale;
	struct sockaddr_in adresse_client;
	struct hostent* ptr_hote;
	char* nomJoueur;
	char* bufferAction;
	int pv;
	int degats;
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
timeval* timeoutRead;

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
 *	@brief initialisation du serveur
 */ 
void initServer(int port);

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
 * @brief Fonction qui génère un joueur et l'ajoute 
 * aux joueurs du jeu
 * @param sock : le socket du joueur
 * @param nom : le nom du joueur
 */
void initJoueur(int sock, char* nom, sockaddr_in cli_addr);

/**
 * @brief envoie un message a tous les joueurs
 * @details log les envois
 * @param message le char* du message
 */
void envoiTous(char* message);

/**
 * @brief verifie que tous les joueurs sont pas morts 
 * @return 1 si ils sont tous morts, 0 sinon
 */
int joueursMorts();

/** 
 * @brief fait une action en fonction du buffer
 * @return char* pour le message
 */
char* action(char* buffer, Joueur* joueur);

/**
 * Fonction auxiliaire pour récupérer les infos du message
 * (utilisée dans les fonctions suivantes)
 */
int getDonnees(char* mesg, int nbData, int data1Pos, int data2Pos, int data3Pos);

/** 
 * Fonction renvoyant la longueur du nom de la source du message
 * @param msg : le char* du message
 * @return int  la longueur du nom de la source
 */
int getSourceLongueur(char* mesg);

/** 
 * Fonction renvoyant la longueur du nom de la cible du message
 * @param msg : le char* du message
 * @return int  la longueur du nom de la cible
 */
int getCibleLongueur(char* mesg);

/** 
 * Fonction renvoyant la longueur des données
 * @param msg : le char* du message
 * @return int  la longueur des données
 */
int getDonneesLongueur(char* mesg);

/** 
 * Fonction renvoyant les points de vie
 * @param msg : le char* du message
 * @return int  les points de vie
 */
int getPointsDeVie(char* mesg, int offset);

/** 
 * Fonction renvoyant le nombre de clients
 * @param msg : le char* du message
 * @return int  le nombre de clients
 */
int getNbClient(char* mesg);

/** 
 * Fonction renvoyant la longueur du nom client
 * @param msg : le char* du message
 * @param offset : taille entete + longueur nom de la source + longueur nom de la cible
 * @return int  la longueur du nom client
 */
int getLongueurNomClient(char* mesg, int offset);

/** 
 * Fonction renvoyant le nom de la source du message
 * @param msg : le char* du message
 * @param longueurEntete : int pour la longueur de l'en-tête du message
 * @return char* le nom de la source
 */
char* getSourceNom(char* mesg, int longueurEntete);

/** 
 * Fonction renvoyant le nom de la cible du message
 * @param msg : le char* du message
 * @param longueurEntete : int pour la longueur de l'en-tête du message
 * @return char* le nom de la cible
 */
char* getCibleNom(char* mesg, int longueurEntete);

/** 
 * Fonction renvoyant le type de modification
 * @param msg : le char* du message
 * @return int  le type de modification
 */
int getTypeDeModification(char* mesg);

/** 
 * génère un message pour les clients
 * @param nomSource le char* du nom de la source
 * @param nomDest le char* du nom de la cible
 * @param type le type du message
 * @return char* le message généré
 */
char* genMessage(char* nomSource, char* nomDest, int type);

/**
 * Fonction qui décode le message et en 
 * fait l'affichage pour le serveur 
 * @param mesg : le char* du message
 */
void decode(char* mesg);

/**
 * Fonction qui gère l'attaque d'un joueur
 * @param joueurCourant : le joueur qui attaque
 * @param nomCible : la cible
 */
void attaque(Joueur* joueurCourant, char* nomCible);

/**
 * Fonction qui gère le soin d'un joueur
 * @param joueurCourant : le joueur qui soigne
 * @param nomCible : la cible
 */
void soin(Joueur* joueurCourant, char* nomCible);

/**
 * Fonction qui vérifie qu'un joueur existe
 * @param nom : le nom du joueur à tester
 * @return le joueur existant ou NULL si il n'existe pas
 */
Joueur* joueurExists(char* nom);
