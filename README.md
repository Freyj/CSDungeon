# CSDungeon

Ceci est un petit jeu en réseau pour un projet de master 1. 

De multiples clients peuvent se connecter à un serveur qui organise un jeu en tour par tour lorsque le nombre de clients max est atteint.

Les clients reçoivent chacun leur tour un token pour qu'ils annoncent au serveur leur action sur le jeu.

Les actions sont : attaquer [cible], soigner [cible], quitter.

Les cibles sont soit "ennemi" pour l'ennemi du jeu, soit les noms des autres clients.

Les clients reçoivent la liste des joueurs au début de la partie

Si un client déconnecte, le token passe au joueur suivant.

## compile
### compile server
gcc ./server/server.c -o csServer -Wall -lpthread
### compile client
gcc ./client/client.c -o csClient -Wall -lpthread

## use 
### use server
./csServer [port]
### use client
./csClient [adresse-serveur] [port] [pseudo]


### Test
test.c permet d'avoir un aperçu des différents types de messages générés par notre protocole d'échange.