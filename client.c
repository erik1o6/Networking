#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage) /* Error Handling Function */
{
    perror(errorMessage);
    exit(1);
}

typedef struct {
char filename[32];
unsigned int size;
} first_message;

int main(int argc, char *argv[])
{

   long sent;
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
     unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */
    first_message m;
    char *echoString;				/* String to echo */

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoString = argv[2];         /* Second arg: string to echo */
    
     pFile= fopen ( echoString , "r+" );
    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    //allocate the mem
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    //fread(echoBuffer,)
    // copy the file into the buffer:
    // result = fread (buffer,1,lSize,pFile);
    // if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    strcpy(m.filename,echoString);
    m.size=lSize;
    
    fread (buffer,1,1024,pFile);

	

    if ((argc < 3))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Filename>\n",
               argv[0]);
       exit(1);
    }



    echoServPort = 10000; /* Use port 10000, as assigned */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);          /* Determine input length */

    /* Send the string to the server */
    if (send(sock, &m, 36, 0) != 36)
        DieWithError("send() sent a different number of bytes than expected");
    for(sent =0;lSize>sent; sent+=1024)
      {
       
       
    if (send(sock, buffer, 1024, 0) != 1024)
      DieWithError("send() sent a different number of bytes than expected");

    fseek (pFile , 1024 , SEEK_CUR);	
    fread (buffer,1,1024,pFile);
      }
    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    printf("Received: ");                /* Setup to print the echoed string */
    while (totalBytesRcvd < echoStringLen)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        printf("%s", echoBuffer);      /* Print the echo buffer */
	printf("\n");
	printf("%s", totalBytesRcvd);
    }

    printf("\n");    /* Print a final linefeed */

    close(sock);
    exit(0);
}
