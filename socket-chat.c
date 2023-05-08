/*
 *  * Author: Varun Pasupuleti
 *   * Description: This program creates a simple TCP client-server communication system using sockets and the poll function. The program can be run as a server or a client, and allows the user to send and receive messages between the two. The server listens for incoming connections from clients on a specified port, and the client connects to the server using the server's IP address and port number. Once the connection is established, the user can type messages into the terminal and send them to the other end of the connection, and can also receive messages from the other end. The poll function is used to wait for input from either the user or the remote end of the connection, and to handle any errors that may occur during the communication. This code can be used as a starting point for building more complex networked applications using TCP sockets in C.
 *    */


#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

void setup_server(int *server_socket, int *client_socket, int port) {
    // create server address
	struct sockaddr_in server_address;

	// creates a socket that waits for the other socket to connect	
    if ((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(1);
    }

	// creates the address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
	// tell OS that this socket will use the address created
    if (bind(*server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind failed");
        exit(1);
    }

	// waits for a connection
    if (listen(*server_socket, 1) == -1) {
        perror("listen failed");
        exit(1);
    }

	//prints info
	system("host $HOSTNAME"); // display all this computer's IP addresses
    printf("Server is listening on port %d\n", port);

	// gets a connection
    if ((*client_socket = accept(*server_socket, NULL, NULL)) == -1) {
        perror("accept failed");
        exit(1);
    }
	
	// disconnect because we have accepted the client socket
    close(*server_socket);
}

void setup_client(int *client_socket, const char *ip_address, int port) {
    struct sockaddr_in server_address;
	
	// creates the client socket
    if ((*client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(1);
    }

	// creates the address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) == -1) {
        perror("inet_pton failed");
        exit(1);
    }

	// connects by taking in the socket, ip, port and size of server_address
    if (connect(*client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect failed");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    int port;
	
	// if arg size 1-2, acts as server
    if (argc == 1 || argc == 2) {
		// gets random port
		srandom(getpid());
		// gets specified port
	    port = argc == 2 ? atoi(argv[1]) : (0xc000 | (random() & 0x3fff));
		// runs server set up
        setup_server(&server_socket, &client_socket, port);
    // if arg size 3, acts as client
	} else if (argc == 3) {
        // gets specified port and sets up client
		port = atoi(argv[2]);
        setup_client(&client_socket, argv[1], port);
    } else {
        fprintf(stderr, "Usage: %s [<ip_address>] [<port>]\n", argv[0]);
        return 1;
    }

	// specifies that poll should monitor the client and standard input
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = client_socket;
    fds[1].events = POLLIN;

    char buffer[BUFFER_SIZE];
    ssize_t num_bytes;

    while (1) {
		// polls the specified descriptors
        int poll_result = poll(fds, 2, 60000);

        if (poll_result > 0) {
			// if from stdin, reads it and posts on client side
            if (fds[0].revents & POLLIN) {
                num_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);
                if (num_bytes > 0) {
                    write(client_socket, buffer, num_bytes);
                }
            }
			// if from client, reads it and posts to stdout
            if (fds[1].revents & POLLIN) {
                num_bytes = read(client_socket, buffer, BUFFER_SIZE);
                if (num_bytes > 0) {
                    write(STDOUT_FILENO, buffer, num_bytes);
                } else {
                    break;
                }
            }
		// if no poll for 60 seconds, breaks loop
        } else if (poll_result == 0) {
            break;
        } else {
            perror("poll failed");
            break;
        }
    }
	// closes client after communication ends
    close(client_socket);
    return 0;
}

