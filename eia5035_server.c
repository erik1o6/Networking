//Written By Erik Arfvidson

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 20   /* Size of receive buffer */
//Structure of m message
typedef struct {
char filename[32];
unsigned int size;
} m_message;

void DieWithError(char *errorMessage) /* Error Handling Function */
{
    perror(errorMessage);
    exit(1);
}

void HandleTCPClient(int clntSocket)
{
    char fileBuffer[RCVBUFSIZE];        /* Buffer for data */
    int recvMsgSize;                    /* Size of received message */
    
	char *fileName;						/* Name of file to write to */
	int size;							/* Size of file, to send back to client */
	FILE* fp;							/* File pointer for writing */
	m_message m;
	char* incData = (char*) malloc(sizeof(char)*1024); 
	int bytesreceived = 0;
	char* newName = (char*) malloc(strlen(m.filename) + 4);
	recvMsgSize = recv(clntSocket, &m, sizeof(m_message), 0);   //recieve the m packet
	
	
	
	//appends rec to the filename
	strcpy(newName, "rec_");          
	strcat(newName, m.filename);
	fp = fopen(newName, "w");
        printf("Received file %s of size %d\n", m.filename, m.size);
    /* Send received string and receive again until end of transmission */
	
    while (bytesreceived < m.size)      /* zero indicates end of transmission */
    {        
        /* listen for the file */
		
		//case when less 1024
        if ((recvMsgSize = recv(clntSocket, incData, 1024, 0)) < 0)
            DieWithError("recv() failed");
        bytesreceived+=recvMsgSize;
        printf("Received byte %d of %d\n", bytesreceived, m.size);
        //larger 1024
        if(m.size-bytesreceived<(1024))
            {fwrite(incData, m.size-bytesreceived, 1, fp);}
        else
            {fwrite(incData, 1024, 1, fp);}
          
    }
    // SEND ACK
	printf("Done receiving file %s\n", m.filename);
	printf("Sending acknowledgment\n");
	send(clntSocket, "Message received.", 18, 0);     
	fclose(fp);
    close(clntSocket);    /* Close client socket */
}

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort = 10000;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 1)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s\n", argv[0]);
        exit(1);
    }

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    	printf("Server listening on address 127.0.0.1, port 10000\n");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s:%i\n", inet_ntoa(echoClntAddr.sin_addr), ntohs(echoClntAddr.sin_port));

        HandleTCPClient(clntSock);
    }
    /* NOT REACHED */
}
