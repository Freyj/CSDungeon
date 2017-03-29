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

int getDonnees(char* mesg, int nbData, int data1Pos, int data2Pos, int data3Pos);
int getSourceLongueur(char* mesg);
int getCibleLongueur(char* mesg);
int getDonneesLongueur(char* mesg);
int getPointsDeVie(char* mesg, int offset);
int getNbClient(char* mesg);
int getLongueurNomClient(char* mesg, int offset);
char* getSourceNom(char* mesg, int longueurEntete);
char* getCibleNom(char* mesg, int longueurEntete);
int getTypeDeModification(char* mesg);
char* genMessage(char* nomSource, char* nomDest, int type);
void decode(char* mesg);
void attaque(Joueur* joueurCourant, char* nomCible);
void soin(Joueur* joueurCourant, char* nomCible);
void tourEnnemi();