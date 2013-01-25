//Written By: Erik Arfvidson

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */
// structure of the m message
typedef struct {
char filename[32];
unsigned int size;
} m_message;

void DieWithError(char *errorMessage) /* Error Handling Function */
{
    perror(errorMessage);
    exit(1);
}

char *substr(const char *pstr, int start, int numchars)
{
	char *pnew = malloc(numchars+1);
	strncpy(pnew, pstr + start, numchars);
	pnew[numchars] = '\0';
	return pnew;
}
int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */ 
    int bytesSent = 0;
    m_message m;
    char *fileName;				     /* Name of file */
    FILE *data;          
    char *strTemp;
    size_t result;
	
	
    if ((argc < 3))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Filename>\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* m arg: server IP address (dotted quad) */
    fileName = argv[2];           /* Second arg: filename */

    echoServPort = 10000; /* Use port 10000, as assigned */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    memset(&m, 0, sizeof(m));
    if(strlen(fileName) > 31)                    /* truncate filename if its too long */
	{ fileName = substr(fileName, 0, 31);	}
    strcpy(m.filename, fileName);            /* put the filename in the m message */
    //printf("the file name =%s\n",m.filename);
    data = fopen(m.filename, "r"); 
    fseek(data, 0, SEEK_END);             /* Calculate the file size */
    m.size = ftell(data);
    rewind(data);
    send(sock, &m, sizeof(m_message), 0);  // find the size of m
    strTemp = (char*) malloc(sizeof(char)*1024);

    
	//Send packet
    do
    {//packets are smaller 1024
        if((m.size-bytesSent)<1024)
        {    
             fread(strTemp, m.size-bytesSent, 1, data);     
             send(sock, strTemp, m.size-bytesSent, 0); 
             bytesSent=m.size;
        }
	else//packets are larger than 1024
        {
            fread(strTemp, 1024, 1, data);
            bytesSent += 1024;
	    send(sock, strTemp, 1024, 0);
        };
    }while(bytesSent != m.size);
	
    char *confirmation = (char*) malloc(18*sizeof(char));
    while(recv(sock, confirmation, 18, 0) == 0)
    {
        //Wait for confitmation
    }
	
    fclose(data);
    close(sock);
    exit(0);
}
