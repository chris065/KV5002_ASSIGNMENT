/*
Controller Class (C)
Version: 1.3
Version Date: 12/05/2019
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
#include <semaphore.h>
#include <assert.h>
#include <pthread.h>

const char *setCommand(char *command)
{
	if(strcmp(command, "1") == 0)
	{
		return "state:?";
	}
	if(strcmp(command, "2") == 0)
	{
		return "condition:?";
	}
	if(strcmp(command, "3") == 0)
	{
		return "terrain:?";
	}
	else
	{
		return "";
	}
}

void finished(int sig)
{
    exit(0);
}

static int fd;
void cleanup(void)
{
    close(fd);
}

size_t protocol(char *incoming, char *outgoing, size_t size)
{
	int i;
	for(i = 0; i<size && incoming[i]!='\0'; i++)
	{
		outgoing[i] = toupper(incoming[i]);
	}
	outgoing[i] = '\0';
	return strlen(outgoing);
}


int messageToDash(char *message[])
{
	char *host = "127.0.1.1";
    	char *port = "65250";
    	struct addrinfo *address;
    	const struct addrinfo hints = {
        	.ai_family = AF_INET,
        	.ai_socktype = SOCK_DGRAM,
    	};

    	int fd, err;

    	err = getaddrinfo(host, port, &hints, &address);
    	if (err) 
	{
        	fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        	exit(1);
    	}
    	fd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (fd == -1) 
	{
        	fprintf(stderr, "error making socket: %s\n", strerror(errno));
        	exit(1);
    	}

    	/* Client message and  */
    	const size_t buffsize = 4096;       /* 4k */
    	char incoming[buffsize], outgoing[buffsize];
    	size_t msgsize;
    	struct sockaddr clientaddr;
    	socklen_t addrlen = sizeof(clientaddr);

    	sprintf(outgoing, message);
    	/*Recieve message from server and parse into variables replace value for fuel and altitude 		with those*/    
   	sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);

	return 0;
	
}

char* requestDataFromServer(char *message[])
{
	const char *command = setCommand(message);
	

	//char *host = argv[1];
	char *host = "127.0.1.1";
	//char *port = argv[2];
	char *rcvPort = "65200";
	
	struct addrinfo *address;
	const struct addrinfo hints =
	{
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM,
	};
	int fd, err;
	
	err = getaddrinfo(host, rcvPort, &hints, &address);
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
	
	strcpy(outgoing, command);
	sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);

	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
	incoming[msgsize] = '\0';
	printf("Reply: %s\n", incoming);

	char *reply = incoming;
	return reply;
}



char* getContactAndSendToDash(char *condition[])
{
	char *str = condition;
	int init_size = strlen(str);
	char * pch;

	pch = strtok(str, ":=%\n");
		
	
	while(pch != NULL)
	{
		//printf("%s\n", pch);
		pch = strtok(NULL, ":=%\n");
	}
	
	/*
	for(int i = 0; i < init_size; i++)
	{
		printf("%s\n", &str[i]);
	}
	*/
	
	//char *statusString[] = {str[17], str[33], str[63]};
	char *strStat[100];
	strcpy(strStat, "fuel:");
	strcat(strStat, &str[17]);
	strcat(strStat, "\n");
	strcat(strStat, "altitude:");
	strcat(strStat, &str[33]);
	strcat(strStat, "\n");
	strcat(strStat, "contact:");
	strcat(strStat, &str[63]);
	strcat(strStat, "\n");
	//puts(strStat);
	

	//printf("%s\n", strStat);
	
	messageToDash(&strStat);
	return &strStat;
}


void controls()
{
    int state = 1;
	const size_t buffsize = 4096;
    char msg[buffsize];
    while(state == 1){
        switch(getchar()){
        case 'w':
            //code to set throttle 100
            sprintf(msg,"command:!\nmain-engine:100\n");
			sendCommand(msg);
            break;
        case 'a':
            //code to set rcs-roll -1
            sprintf(msg,"command:!\nrcs-roll:-1\n");
			sendCommand(msg);
            break;
        case 's':
            //code to set throttle 0
            sprintf(msg,"command:!\nmain-engine:0\n");
			sendCommand(msg);
            break;
        case 'd':
            //code to set rcs-roll 1
            sprintf(msg,"command:!\nrcs-roll:1\n");
			sendCommand(msg);
	 case 'c':
            //code to set rcs-roll 0
            sprintf(msg,"command:!\nrcs-roll:0\n");
			sendCommand(msg);
            break;
		case 'q':
			//code to quit
			state = 0;
			break;
        }
    }
    //check for any keypress
    //send message to lander server  
}
void sendCommand(char command[4096])
{
    char *host = "127.0.1.1";
    char *port = "65200";
    struct addrinfo *address;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int fd, err;

    err = getaddrinfo(host, port, &hints, &address);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fprintf(stderr, "error making socket: %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char incoming[buffsize], outgoing[buffsize];
    size_t msgsize;
    struct sockaddr clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    strcpy(outgoing, command);    
    sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
}

int sendDataToDashLoop()
{
	pthread_t thread1;
	char* condition = requestDataFromServer("2");
	
	while(true)
	{
		
		if(pthread_create(&thread1, NULL, getContactAndSendToDash, condition))
		{
			fprintf(stderr, "failed to create the thread1");
			return 1;
		}

		requestDataFromServer("2");

		
		if(pthread_join(thread1, NULL))
		{
			fprintf(stderr, "failed to join the thread1");
			return 2;
		}
		sleep(1);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	pthread_t thread[3];
	//const char *command = setCommand(argv[1]);
	int rc;

	//constantantly request and send data to the dash
	rc = pthread_create(&thread[0], NULL, sendDataToDashLoop, NULL);
	assert(rc == 0);
	//get the controls of the player and send it to the server
	rc = pthread_create(&thread[1], NULL, controls, NULL);
	assert(rc == 0);
	//Need a thread to send key presses to the file
	
	while(true)
	{
	}
	exit(0);
	
	
	return 0;
}
//mehtod to send message to the dash
//take a paremeter that is the message




