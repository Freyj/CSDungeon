
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <linux/types.h>
#include <string.h>

/*
gcc myProgram.o -o myProgram -lpthread


int pthread_create(pthread_t * pth, pthread_attr_t *att, void * (*function), void * arg);

pthread_t thr;
pthread_create(&thr, NULL, &hello, NULL);

*/
/* Structure pour les armes */
typedef struct arme {
	char* nom;
	bool estMagique;
	int degats;
	int precision;
}arme;

/* Structure pour stocker les infos du client */
typedef struct infoclient {
	/* nom  */
	char* nom;
	/* caractéristiques */
	int pv;
	int pvMax;
	int exp;
	int niveau;
	int force;
	int magie;
	int technique;
	int vitesse;
	int chance;
	int defense;
	int resistance;
	arme arme;
	/*socket associé */

}infoclient;

/* Thread pour faire des trucs */
pthread_t* ptr_thread;
infoclient* infoTest;

/* Fonction qui fait tourner le tour*/
void* tourDeJeu(){
	printf("%10s", "tour");
	infoTest = malloc(sizeof(infoclient));
	infoTest->nom = "Bouh";
	if (!strcmp(infoTest->nom, "Bouh")) {
		printf("%10s", "test");
	}
}

/* Fonction qui fait tourner le tour*/
void* client(){
	printf("%10s", "CLIENT\n");
}



int main(void) {
	int nbClient;
	nbClient = 3;

	pthread_t  threadJeu;
	pthread_t* threadClients;
	threadClients = calloc(nbClient, sizeof(pthread_t));

	int mainProc = pthread_create(&threadJeu, NULL, &tourDeJeu, NULL);
	int* clientProc;
	clientProc = calloc(nbClient, sizeof(int));
	int i = 0;
	while(i < nbClient){
		clientProc[i] = pthread_create(&threadClients[i], NULL, &client, NULL);
		i = i + 1;
	}

	if (mainProc == 0) {
   		(void) pthread_join(threadJeu, NULL);
   		i = 0;
		while(i < nbClient){
   			(void) pthread_join(threadClients[i], NULL);
   			++i;
   		}
		printf("%10s","Thread de jeu terminé\n\0");

	} else {
		printf("%10s", mainProc);
		printf("%10s","Lancement du thread de jeu échoué\n\0");
	}
	
	free(clientProc);
	free(threadClients);

	return 0;
}
