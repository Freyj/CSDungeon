
#include <stdlib.h>
#include <stdio.h>
//#include <linux/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
#include <string.h>

//#3E6EB7

int digit_to_int(char d){
	char str[2];
	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}

int getSourceLongueur(char* mesg){
	//printf("int getSourceLongueur(char* mesg) \n");
	int nomSourceLongueur;
	nomSourceLongueur = 0;
	nomSourceLongueur = nomSourceLongueur + 10 * digit_to_int(mesg[1]);
	nomSourceLongueur = nomSourceLongueur + digit_to_int(mesg[2]);
	//printf("nomSourceLongueur : %i\n", nomSourceLongueur);
	return nomSourceLongueur;
}

int getCibleLongueur(char* mesg){
	//printf("int getCibleLongueur(char* mesg) \n");
	int nomCibleLongueur;
	nomCibleLongueur = 0;
	nomCibleLongueur = nomCibleLongueur + 10 * digit_to_int(mesg[3]);
	nomCibleLongueur = nomCibleLongueur + digit_to_int(mesg[4]);
	//printf("nomCibleLongueur : %i\n", nomCibleLongueur);
	return nomCibleLongueur;
}

int getDonneesLongueur(char* mesg){
	//printf("int getDonneesLongueur(char* mesg) \n");
	int donneesLongueur;
	donneesLongueur = 0;
	donneesLongueur = donneesLongueur + 100 * digit_to_int(mesg[6]);
	donneesLongueur = donneesLongueur + 10 * digit_to_int(mesg[7]);
	donneesLongueur = donneesLongueur + digit_to_int(mesg[8]);
	//printf("donneesLongueur : %i\n", donneesLongueur);
	return donneesLongueur;
}

int getPointsDeVie(char* mesg, int offset){
	//printf("int getPointsDeVie(char* mesg, int offset) \n");
	int pointsDeVie;
	pointsDeVie = 0;
	pointsDeVie = pointsDeVie + 100 * digit_to_int(mesg[offset]);
	pointsDeVie = pointsDeVie + 10 * digit_to_int(mesg[offset + 1]);
	pointsDeVie = pointsDeVie + digit_to_int(mesg[offset + 2]);
	//printf("pointsDeVie : %i\n", pointsDeVie);
	return pointsDeVie;
}

int getNbClient(char* mesg){
	//printf("int getNbClient(char* mesg) \n");
	int nbClient;
	nbClient = 0;
	nbClient = nbClient + 10 * digit_to_int(mesg[1]);
	nbClient = nbClient + digit_to_int(mesg[2]);
	//printf("nbClient : %i\n", nbClient);
	return nbClient;
}

int getLongueurNomClient(char* mesg, int offset){
	//printf("int getLongueurNomClient(char* mesg) \n");
	int longueurNomClient;
	longueurNomClient = 0;
	longueurNomClient = longueurNomClient + 10 * digit_to_int(mesg[offset]);
	longueurNomClient = longueurNomClient + digit_to_int(mesg[offset + 1]);
	//printf("longueurNomClient : %i\n", longueurNomClient);
	return longueurNomClient;
}


void decode(char* mesg) {
	int nomSourceLongueur;
	int nomCibleLongueur;
	int donneesLongueur;
	int typeDeModification;
	int pointsDeVie;
	char* nomSource;
	char* nomCible;
	int longueurEntete;
	longueurEntete = 9;
	printf("\t\t");
	printf(mesg);
	printf("\n");
	if(mesg[0] == '0'){									//0
		//connexion
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		//nomCibleLongueur inutile;						//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		printf("CONNEXION LongueurNomSource : %i LongueurDonnees : %i \nNomSource : %s\n",
		 nomSourceLongueur, donneesLongueur, nomSource);
		free(nomSource);
	}else if(mesg[0] == '1'){							//0
		//attaque
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("ATTAQUE LongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nPV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '2'){							//0
		//soigne
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("SOIGNE LongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nPV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '3'){							//0
		//notification
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		typeDeModification = digit_to_int(mesg[5]);		//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);

		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("NOTIFICATION LongueurNomSource : %i LongueurNomCible : %i LongueurDonnees : %i \nTypeModifi : %i PV : %i NomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, typeDeModification,
		 pointsDeVie, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '4'){							//0
		//mort
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		//donneesLongueur = getDonneesLongueur(mesg);	//6 | 7 | 8
		nomSource = calloc(nomSourceLongueur+1, 1);
		nomCible = calloc(nomCibleLongueur+1, 1);
		strncpy(nomSource, &mesg[longueurEntete], nomSourceLongueur);
		strncpy(nomCible, &mesg[longueurEntete + nomSourceLongueur], nomCibleLongueur);
		printf("MORT LongueurNomSource : %i LongueurNomCible : %i \nNomSource : %s NomCible : %s\n",
		 nomSourceLongueur, nomCibleLongueur, nomSource, nomCible);
		free(nomSource);
		free(nomCible);
	}else if(mesg[0] == '5'){							//0
		//Liste de client
		int nbClient;
		nbClient = getNbClient(mesg);					//1 | 2 NBCLIENT
		int longueurNomClient[nbClient];
		//char*
		int offset = 0;
		//int sommeLongueur = 0;
		for(int i = 0; i < nbClient; ++i){
			offset = 2 * i + 3;
			longueurNomClient[i] = getLongueurNomClient(mesg, offset);
			printf("longueurNomClient a la position : %i -- %i\n", i, longueurNomClient[i]);
			//sommeLongueur = sommeLongueur + longueurNomClient[i];
		}
		offset = offset + 2;
		char * nomsClients[nbClient];
		for(int i = 0; i < nbClient; ++i){		
			nomsClients[i] = calloc(longueurNomClient[i]+1, 1);
			strncpy(nomsClients[i], &mesg[offset], longueurNomClient[i]);
			printf("nomClient a la position : %i -- %s\n", i, nomsClients[i]);
			offset = longueurNomClient[i] + offset;
		}

	}else{
		perror("erreur : message errone.");
		exit(1);
	}
	printf("\n");
}


int main(){
	char* m1 = calloc(100, 1);
	m1 = "006000003ELDRAD";
	decode(m1);
	//attaque
	//nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
	//nomCibleLongueur = getCibleLongueur(mesg);	//3 | 4
	//typeDeModification inutile					//5
	//donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8

	char* m2 = calloc(100, 1);
	//				  12 15 18
	//				10 13 16 19
	//	  0123456789 11 14 17
	m2 = "106030003ELDRADBOB002";
	decode(m2);
	char* m3 = calloc(100, 1);
	m3 = "206030003ELDRADBOB002";
	decode(m3);
	char* m4 = calloc(100, 1);
	m4 = "306030003ELDRADBOB002";
	decode(m4);
	char* m5 = calloc(100, 1);
	m5 = "406030000ELDRADBOB";
	decode(m5);
	char* m6 = calloc(100, 1);
	m6 = "506060302100405ELDRADBOBXILUMINACALIJULEFREYA";
	decode(m6);

	free(m1);
	free(m2);
	free(m3);
	free(m4);
	free(m5);
	free(m6);
}

