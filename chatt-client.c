#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>


void trim_message(char * arr, int length);

/* receive_messages() is a void function that works in conjunction with pthread to loop through and receive messages and print them out to 
 * stdout for the client.
 */
void * receive_messages(void *my_sock)
{
    int sock = *((int *) my_sock);
    int len;
    char message[500];
    while (1)
    {
        memset(&message, 0, sizeof(message));
        len = recv(sock, message, 500, 0);
        if (len == 0)
        {
            break;
        }
        message[len] = '\0';
        fputs(message, stdout);
    }

    pthread_detach(pthread_self());
    return NULL;
}

/*  trim_message() will remove the newline character in a char array which is used to send the message without the newline which is received from
 *  fgets()
 */
void trim_message(char * arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}


int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char send_msg[500];
    int len;
    pthread_t pthread;
    int error_check = 1;
    char name[30];
    char buffer[500];
    int exit_status;
    
    if (argc != 3)    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s requires <ip address> [<Echo Port>] for server\n",
               argv[0]);
       exit(1);
    }
    
    printf("CHATT-CLIENT, connect only to CHATT-SERVER\n");
    printf("NOTE: EXIT SERVER by typing \'exit\' or by ctrl-c\n");

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]); /* Use given port, */
    while (error_check)
    {
        printf("Enter your name: ");
        fgets(name, 30, stdin);
        len = strlen(name);
        if (len > 25)
        {
            printf("name is too long\n");
            char ch;
            while ( ( ch = getchar() ) != '\n' && ch != EOF );
        }
        else if (len <= 2)
        {
            printf("name is too short\n");
        }
        else
        {
            error_check = 0;
        }
    }

    trim_message(name, 30);

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        perror("connect() failed");
        exit(1);
    }
    send(sock, name, strlen(name), 0);
    pthread_create(&pthread, NULL, (void *) receive_messages, &sock);
    
    while (1)
    {
        memset(&buffer, 0, sizeof(buffer));
        memset(&send_msg, 0, sizeof(send_msg));
        fgets(buffer, 500, stdin);
        strcpy(send_msg, name);
        strcat(send_msg, " said: ");
        strcat(send_msg, buffer);
        trim_message(buffer, 500);
        if (strcmp(buffer, "exit") == 0)
        {   
            exit_status = 1;
            break;
        }
        len = send(sock, send_msg, strlen(send_msg), 0);
        if (len < 0)
        {
            printf("Message not send\n");
        }
    }

    while (1)
    {
        if (exit_status)
        {
            printf("Left SERVER\n");
            break;
        }
    }
    close(sock);
}
