/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.1
 * @brief : 
*/

/*-----------------------------------------------------------
  Client a lancer apres le serveur avec la commande :
  client <adresse-serveur> <pseudo>
  ensuite les commandes sont du type : 
  [attaquer|soigner] cible [option]
  le serveur gèrera l'ordre
  ------------------------------------------------------------*/
#include "client.h"

int main(int argc, char **argv) {
	int  socket_descriptor; 	/* descripteur de socket */
	int  longueur; 				/* longueur d'un buffer utilisé */

	sockaddr_in adresse_locale; /* adresse de socket local */

	hostent * ptr_host;  		/* info sur une machine hote */
	servent * ptr_service;  	/* info sur service */
	char buffer[256];
	char * prog; 				/* nom du programme */
	char * host; 				/* nom de la machine distante */
	char * mesg; 				/* message envoyé */
	int port; /*le port de connexion*/
	if (argc != 4) {
		perror("usage : client <adresse-serveur> <port> <message-a-transmettre>");
		exit(1);
	}
	prog = argv[0];
	host = argv[1];
	port = atoi(argv[2]);
	mesg = argv[3];
	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);
	printf("message envoye      : %s \n", mesg);
	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}
	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

	adresse_locale.sin_port = htons(port);

	printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
	}

	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
	}

	printf("connexion etablie avec le serveur. \n");
	printf("envoi d'un message au serveur. \n");

	/* envoi du message vers le serveur */
	if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}

	printf("message envoye au serveur. \n");
	/* lecture de la reponse en provenance du serveur */
	while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		write(1,buffer,longueur);
	}

	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	exit(0);  
}
