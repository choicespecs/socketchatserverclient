#include <sys/wait.h>       /* for waitpid() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <string.h>     /* for memset() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char global_name[30];
char server_message[300];
int clients[10];
int current_clients = 0;
int max = 3;

/* remove_client(int socket) will take out clients listed within the client list when clients enter or leave
 * return is void, so will only remove the appropriate socket.
 */
void remove_client(int socket)
{
    int index;
    int index_remove = 0;
    for (index = 0; index < current_clients; index++)
    {
        if (clients[index] == socket)
        {
            index_remove = index;
        }
    }
    current_clients--;
    for (index = index_remove; index < current_clients; index++)
    {
        clients[index] = clients[index + 1];
    }
}

/* sendtoall(char * msg, int socket) will send a message to all other clients within the server, except the one who is sending the 
 * message.
 */
void sendtoall(char *msg, int socket)
{   
    int index;
    pthread_mutex_lock(&mutex);
    for (index = 0; index < current_clients; index++)
    {
        if (clients[index] != socket)
        {
            if (send(clients[index], msg, strlen(msg), 0) < 0) 
            {
                printf("sending failure\n");
                continue;
            }
        }
    }

    pthread_mutex_unlock(&mutex);
}

/* client_handler is a void function that is used in conjunction together with pthreads to create a function that works together to handle the client
 * this function will loop continuously in the server and will send out all messages to the clients within the server.
 */
void * client_handler(void *client_sock)
{
    int sock = *((int *) client_sock);
    char user_name[30];
    strcpy(user_name, global_name);
    memset(&global_name, 0, sizeof(global_name));
    if (current_clients > 1)
    {
        sendtoall(server_message, sock);   
    }
    memset(&server_message, 0, sizeof(server_message));
    char msg[500];
    int len;
    while(1)
    {
        len = recv(sock, msg, 500, 0);
        if (len == 0)
        {
            pthread_mutex_lock(&mutex);
            sprintf(server_message, "----- User: [%s] has left the server -----\n", user_name);
            printf(server_message);
            pthread_mutex_unlock(&mutex);
            sendtoall(server_message, sock);
            break;
        }
        msg[len] = '\0';
        sendtoall(msg, sock);
    }
    remove_client(sock);
    memset(&server_message, 0, sizeof(server_message));
    printf("Waiting for connections from clients \t Current Clients (%d / %d) \n", current_clients, max);
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char *argv[])
{
    int servsock;                    /* socket descriptor for server */
    int clntSock;
    unsigned short echoservport;     /* server port */
    unsigned int clntLen;
    struct sockaddr_in echoServAddr; 
    struct sockaddr_in echoClntAddr;
    pthread_t pthread;
    int error_check = 1;
    int valid_num;
    int client_num;
    int len;

    if (argc != 2)     /* test for correct number of arguments */
    {
        fprintf(stderr, "usage:  %s <server port>\n", argv[0]);
        exit(1);
    }

    printf("CHATT-SERVER will work with CHATT-CLIENT\n");
    printf("NOTE: EXIT only by ctrl-c\n");
    while (error_check)
    {
        printf("How many clients do you want (MAX = 10): ");
        valid_num = scanf("%d", &client_num);
        if (valid_num == EOF || valid_num == 0)
        {
            printf("Input is not readable, input an integer from 2 - 10\n");
            while(fgetc(stdin) != '\n');
            continue;
        }
        if (client_num < 2 || client_num > 10)
        {
            printf("Input is not valid, clients must be from 2 - 10\n");
            continue;
        }
        else 
        {
            max = client_num;           
            error_check = 0;
        }
    }

    echoservport = atoi(argv[1]);  /* first arg:  local port */

    if ( ( servsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) ) < 0)
    {
        perror("Socket() Failed");
        exit(1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoservport);              /* Local port */

    /* Bind to the local address */
    if (bind(servsock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        perror("bind() failed");
        exit(1);
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servsock, max) < 0)
    {
        perror("listen() failed");
        exit(1);
    }

    for (;;) /* run forever */
    {
        clntLen = sizeof(echoClntAddr);
        if (current_clients != max)
        {
            printf("Waiting for connections from clients \t Current Clients (%d / %d) \n", current_clients, max);
        }
        if (current_clients == max)
        {
            printf("Connect has reached max clients, will reject further connections \tCurrent Clients (%d / %d)\n", current_clients, max);
        }
        /* Wait for a client to connect */
        if (( clntSock = accept(servsock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
        {
            perror("accept() failed");
            exit(1);
        }
        if (current_clients != max)
        {
            pthread_mutex_lock(&mutex);
            len = recv(clntSock, global_name, 30, 0);
            global_name[len] = '\0';
            sprintf(server_message, "----- User: [%s] has joined the server -----\n", global_name);
            printf(server_message);
            clients[current_clients] = clntSock;
            current_clients++;
            pthread_create(&pthread, NULL, (void*) client_handler, &clntSock);
            pthread_mutex_unlock(&mutex);
        } 
        else
        {
            close(clntSock);
            continue;
        }
    }
}
