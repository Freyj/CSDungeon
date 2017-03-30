all:	client server
client: ./client/client.c
	gcc ./client/client.c -ggdb -o csClient -lpthread
server: ./server/server.c
	gcc ./server/server.c -ggdb -o csServer -lpthread

clean:
	rm csClient csServer logServeur.log