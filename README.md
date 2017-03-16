# CSDungeon

##compile server
gcc ./server/server.c -o csServer -Wall -lpthread
##compile client
gcc ./client/client.c -o csClient -Wall -lpthread

##use server
./csServer <port>
##use client
./csClient <adresse-serveur> <port> <message-a-transmettre>