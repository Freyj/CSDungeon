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

#define TAILLE_MAX_NOM 13
#define MAX_JOUEURS 16
#define TAILLE_BUFFER 256

// pour compter les joueurs
static int nbJoueursCourants = 0;
//pour les identifier
static int joueurIdCompteur = 0;
//int du socket
static int descripteurSocket; /* socket descriptor */
//int identifiant le joueur dont c'est le tour
static int joueurTour = 0;

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
	//mutex pour éviter la modif d'un joueur
	pthread_mutex_t mutex_Joueur;
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


char* heure();
void runLog(char* erreur, int threadNb);
void runLogInt(int erreur, int threadNb);

void renvoi(int sock);
Ennemis* genEnnemis(Ennemis* en);
int ennemisElimines(Ennemis* en);
void attaque(int degats);
void soigner();
void retraitJoueur(Joueur* joueur);
void* tourDeJeu(void* arg);
void action(int sock, Ennemis* en);
void initServer(int port);
void envoiTous(char* message);
void gestionSignal(int nomSignal);
void* loopJoueur(void* arg);
void ajoutJoueur(Joueur* joueur);
Joueur* initJoueur(sockaddr_in adresse_locale, int nouv_sock);

void sendMessage(int port, char* host, char* mesg);
int digit_to_int(char d);
int getSourceLongueur(char* mesg);
int getCibleLongueur(char* mesg);
int getDonneesLongueur(char* mesg);
int getPointsDeVie(char* mesg, int offset);
int getNbClient(char* mesg);
int getLongueurNomClient(char* mesg, int offset);
void decode(char* mesg);
char * genMessage (int port, char* host, char* nomSource, char* nomDest, int type, int tDm);
