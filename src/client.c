/**
 * @Authors : Charlène Servantie, Charles-Eric Begaudeau
 * @Date : 2017
 * @Version : 0.1
 * @brief : 
*/

/* Structure pour les armes */
typedef struct arme {
	char* nom;
	int estMagique;
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


