/*
Controller Class (C)
Version: 1.1
Version Date: 29/03/2019
Author: Chris Bennett (w17004754)
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>


int main(int argc, char *argv[])
{
	//char *host = argv[1];
	char *host = "127.0.1.1";
	//char *port = argv[2];
	char *port = "65200";
	
	struct addrinfo *address;
	const struct addrinfo hints =
	{
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM,
	};
	int fd, err;
	
	err = getaddrinfo(host, port, &hints, &address);
	if(err)
	{
		fprintf(stderr, "Error getting the address: %s\n", gai_strerror(err));
		exit(1);
	}
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd == -1)
	{
		fprintf(stderr, "Error making the socket: %s\n", strerror(errno));
		exit(1);
	}
	
	const size_t buffsize = 4096;
	char incoming[buffsize], outgoing[buffsize];
	size_t msgsize;
	struct sockaddr clientaddr;
	socklen_t addrlen = sizeof(clientaddr);
	
	strcpy(outgoing, argv[1]);
	sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);

	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
	incoming[msgsize] = '\0';
	printf("Reply: %s\n", incoming);
}


