
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
	if (infoTest->nom = "Bouh") {
		printf("%10s", "test");
	}

}


int main(void) {

	ptr_thread = malloc(sizeof(pthread_t));

	int mainProc = pthread_create(ptr_thread, NULL, &tourDeJeu, NULL);

	if (mainProc == 0 ) {
		printf("%10s","Thread de jeu terminé\0");

	} else {
		printf("%10s", mainProc);
		printf("%10s","Lancement du thread de jeu échoué\0");
	}
	return 0;
}
