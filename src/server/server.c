/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.1
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
typedef struct infoclient {
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
	int sock;
}infoclient;


typedef struct _donneesThread{
 // Server *server;
  int sock_send;
  int sock_read;
  int status;
  char* buffer_send;
  char* buffer_read;
  pthread_t thread_read;
  pthread_t  thread_send;
  pthread_mutex_t mutex_data;
} donneesThread;


typedef struct _ennemis{
	int pvEn1;
	int pvEn2;
	int pvEn3;
} Ennemis;

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
 */
void runLog(char* erreur) {
  FILE* file = fopen("logServeur.log", "a");
  if (file == NULL) {
	printf("Erreur d'ouverture de fichier de log!\n");
	exit(1);
  }
  fprintf(file, "%s %s : %s \n", "[**Serveur**]", heure(), erreur);
  fclose(file);
}

/*------------------------------------------------------*/
/*------------------------------------------------------*/
void renvoi(int sock) {
	char buffer[256];
	int longueur;
	if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) {
		return;
	}
	//on clear le buffer avant la réception
	/* printf("message lu : %s \n", buffer);
	buffer[0] = 'R';
	buffer[1] = 'E';
	buffer[longueur] = '#';
	buffer[longueur+1] ='\0';
	printf("message apres traitement : %s \n", buffer);  
	printf("renvoi du message traite.\n");
	write(sock,buffer,strlen(buffer)+1);    
	printf("message envoye. \n");
	*/
	runLog(buffer);


	printf("message lu: %s \n", buffer);

	memset(&buffer, "\0", 1);
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

/**
 * @brief fonction vérifiant qu'un groupe n'est pas mort
 */

int ennemisElimines(Ennemis* en) {
	runLog(en->pvEn1);
	if ((en->pvEn1 < 1) && (en->pvEn2 < 1) && (en->pvEn3 < 1)) {
		return 1;
	}
	return 0;
}
/**
 * @brief fonction lançant une attaque sur un groupe
 */
void attaque(Ennemis* en, int degats) {
	/*if (ennemisElimines(en) == 0) {
		if (en->pvEn1 > 0 ) {
			en->pvEn1 = en->pvEn1 - degats;
			runLog("ennemi 1 tapé");
		}
		else if(en->pvEn2 > 0 ) {
			en->pvEn2 = en->pvEn2 - degats;
			runLog("ennemi 2 tapé");
		}
		else if(en->pvEn3 > 0 ) {
			en->pvEn3 = en->pvEn3 - degats;
			runLog("ennemi 3 tapé");
		}
	}
	runLog("ennemis tues");
	*/
}


/**
 * @brief fonction qui soigne
 */
void soigner(){

}



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
	char* reponse;

	if (strcmp(buffer, "Attaque") == 0) {
		//attaque(en, 2);
		reponse = "Vous avez attaqué";
		write(sock,reponse,strlen(reponse)+1);   
	}
	if (strcmp(buffer, "Soigner") == 0) {
		soigner();
		reponse = "Vous vous êtes soigné";
		write(sock,reponse,strlen(reponse)+1);   
	}
	memset(&buffer, "\0", 1);
}


int main(int argc, char **argv) {
	if (argc == 2) {
		int 
		socket_descriptor, /* descripteur de socket */
		nouv_socket_descriptor, /* [nouveau] descripteur de socket */
		longueur_adresse_courante; /* longueur d'adresse courante d'un client */
	
		sockaddr_in 
			adresse_locale, /* structure d'adresse locale*/
			adresse_client_courant; /* adresse client courant */
	
		hostent* ptr_hote; /* les infos recuperees sur la machine hote */
		servent* ptr_service; /* les infos recuperees sur le service de la machine */
		char machine[TAILLE_MAX_NOM+1]; 
		//generation d'un grp d'ennemi
		//FIXME: probleme de malloc 
		Ennemis* groupeEnnemis = malloc(sizeof(Ennemis));
		groupeEnnemis  = genEnnemis(groupeEnnemis);

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
				action(nouv_socket_descriptor, groupeEnnemis);
//				renvoi(nouv_socket_descriptor);

				close(nouv_socket_descriptor);
				++i;
			}
		}
		free(groupeEnnemis);
	}
	else {
		perror("La bonne commande est ./csdungeonServ [port]");
	}
	return 0;
}