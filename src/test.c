
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

//#3E6EB7

int digit_to_int(char d){
	char str[2];

	str[0] = d;
	str[1] = '\0';
	return (int) strtol(str, NULL, 10);
}

int getSourceLongueur(char* mesg){
	printf("int getSourceLongueur(char* mesg) \n");
	int nomSourceLongueur;
	nomSourceLongueur = 0;
	nomSourceLongueur = nomSourceLongueur + 10 * digit_to_int(mesg[1]);
	nomSourceLongueur = nomSourceLongueur + digit_to_int(mesg[2]);
	printf("nomSourceLongueur : %i\n", nomSourceLongueur);
	return nomSourceLongueur;
}

int getCibleLongueur(char* mesg){
	printf("int getCibleLongueur(char* mesg) \n");
	int nomCibleLongueur;
	nomCibleLongueur = 0;
	nomCibleLongueur = nomCibleLongueur + 10 * digit_to_int(mesg[3]);
	nomCibleLongueur = nomCibleLongueur + digit_to_int(mesg[4]);
	printf("nomCibleLongueur : %i\n", nomCibleLongueur);
	return nomCibleLongueur;
}

int getDonneesLongueur(char* mesg){
	printf("int getDonneesLongueur(char* mesg) \n");
	int donneesLongueur;
	donneesLongueur = 0;
	donneesLongueur = donneesLongueur + 100 * digit_to_int(mesg[6]);
	donneesLongueur = donneesLongueur + 10* digit_to_int(mesg[7]);
	donneesLongueur = donneesLongueur + digit_to_int(mesg[8]);

	printf("donneesLongueur : %i\n", donneesLongueur);
	return donneesLongueur;
}

int getPointsDeVie(char* mesg, int pos){
	printf("int getPointsDeVie(char* mesg, int pos) \n");
	int pointsDeVie;
	pointsDeVie = 0;
	pointsDeVie = pointsDeVie + 100 * digit_to_int(mesg[pos + 1]);
	pointsDeVie = pointsDeVie + 10 * digit_to_int(mesg[pos + 2]);
	pointsDeVie = pointsDeVie + digit_to_int(mesg[pos + 3]);
	printf("pointsDeVie : %i\n", pointsDeVie);
	return pointsDeVie;
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
	printf(mesg);
	printf("\n");
	if(mesg[0] == '0'){									//0
		//connexion
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		//nomCibleLongueur inutile;						//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		printf("CONNEXION SL : %i DL : %i NS : %s\n",
		 nomSourceLongueur, donneesLongueur, nomSource);
	}else if(mesg[0] == '1'){							//0
		//attaque
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
				printf("nomSourceLongueur : %i\n", nomSourceLongueur);
				printf("mesg + longueurEntete : %i\n", mesg + longueurEntete);
				printf("nomSourceLongueur : %i\n", nomSourceLongueur);
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
				printf("111\n");

		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		printf("111\n");

		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
				printf("111\n");

		printf("ATTAQUE SL : %i CL : %i DL : %i PV : %i NS : %s NC : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, 
		 pointsDeVie, nomSource, nomCible);
	}else if(mesg[0] == '2'){							//0
		//soigne
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		printf("nomSourceLongueur : %i\n", nomSourceLongueur);
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		printf("111\n");
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("SOIGNE SL : %i CL : %i DL : %i PV : %i NS : %s NC : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, 
		 pointsDeVie, nomSource, nomCible);
	}else if(mesg[0] == '3'){							//0
		//notification
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		typeDeModification = digit_to_int(mesg[5]);				//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		pointsDeVie = getPointsDeVie(mesg, longueurEntete + nomSourceLongueur + nomCibleLongueur);
		printf("NOTIFICATION SL : %i CL : %i DL : %i TDM : %i PV : %i NS : %s NC : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, typeDeModification, 
		 pointsDeVie, nomSource, nomCible);
	}else if(mesg[0] == '4'){							//0
		//mort
		nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		donneesLongueur = getDonneesLongueur(mesg);		//6 | 7 | 8
		strncpy(nomSource, mesg + longueurEntete, nomSourceLongueur);
		strncpy(nomCible, mesg + longueurEntete + nomSourceLongueur, nomCibleLongueur);
		printf("MORT SL : %i CL : %i DL : %i NS : %s NC : %s\n",
		 nomSourceLongueur, nomCibleLongueur, donneesLongueur, 
		 nomSource, nomCible);
	}else{
		perror("erreur : message errone.");
		exit(1);
	}
}


int main(){
	char* m1 = calloc(100, 1);
	m1 = "006000003ELDRAD";
	//decode(m1);
		//attaque
		//nomSourceLongueur = getSourceLongueur(mesg);	//1 | 2
		//nomCibleLongueur = getCibleLongueur(mesg);		//3 | 4
		//typeDeModification inutile					//5
		//donneesLongueur = getCibleLongueur(mesg);		//6 | 7 | 8

	char* m2 = calloc(100, 1);
	m2 = "106030002ELDRADBOB20";
	decode(m2);
	char* m3 = calloc(100, 1);
	m3 = "206000003ELDRAD";
	decode(m3);
	char* m4 = calloc(100, 1);
	m4 = "306000003ELDRAD";
	decode(m4);
	char* m5 = calloc(100, 1);
	m5 = "406000003ELDRAD";
	decode(m5);

}

