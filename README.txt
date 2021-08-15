FEP: Chat Socket Server and Client

Created by:
Christopher Lee

1. Introduction
-------------------
This is a both a chat socket and server that utilizes sockets and pthreads to connect and work together to echo messages. This server will highlight information
such as current users, and will have some form of customization such as allowing a certain amount of users unto a specific server or "chat room". The max numbers is 10,
since I could not test such an amount, so i chose the arbitary number just to check and test.

2. Installation.
--------------------
This file will require the following files for installation.

    a) chatt-server.c
    b) chatt-client.c

A makefile has been provided to assist with the installation of the file (as well as
included the functionality to remove all relevant files).

If any problems occur with the makefile compilation, you should be able to create an executable
using the following command with gcc:
    "gcc -Wall -g chatt-server.c -o -lpthread chatt-server"
    "gcc -Wall -g chatt-client.c -o -lpthread chatt-client"


3. Files / Methods
--------------------
The file is separated only into two main files.

    a) chatt-server.c
    b) chatt-client.c

a) chatt-server.c
    This file contains the server section of the code. This file requires an argument that will be the port that will be listened to for the chat server. The server
    will give further information such as the number of clients, connection status, and other crucial information.

b) chatt-client.c
    This file will contain the client file that will connect to the server. This file requires both the IP and port to connect to the chat server. Once connected the chat client
    should be able to communicate and be notifed of other users and other information within the chat-server.

4. References
----------------
pthreads:
://man7.org/linux/man-pages/man7/pthreads.7.html
https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
https://www.geeksforgeeks.org/multithreading-c-2/
https://stackoverflow.com/questions/22427007/difference-between-pthread-exit-pthread-join-and-pthread-detach
https://www.youtube.com/watch?v=d9s_d28yJq0
https://www.youtube.com/watch?v=ynCc-v0K-do

pthread mutex lock:
://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_mutex_lock.html
https://stackoverflow.com/questions/14888027/mutex-lock-threads

sockets:
https://www.geeksforgeeks.org/socket-programming-cc/
https://man7.org/linux/man-pages/man2/socket.2.html
https://www.binarytides.com/socket-programming-c-linux-tutorial/

recv and send
https://linux.die.net/man/2/recv
https://linux.die.net/man/2/send
