
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


int main(){

	int i;
	i = 0;
	int j;
	j = 0;
	printf("i : %i\n", i);
	printf("i++ : %i\n",i++);
	printf("i : %i\n", i);
	printf("j : %i\n", j);
	printf("++j : %i\n",++j);
	printf("j : %i\n", j);

}

