#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#include "message.h"
#include "socket.h"

void * handle_connection(void* arg) {
  int client_socket_fd = * (int*)arg;
  free(arg);

  while (1) {
    // Read a message from the client
    char* message = receive_message(client_socket_fd);
    if (message == NULL) {
      perror("Failed to read message from client");
      break;
    }

    // Print the message
    printf("Client sent: %s\n", message);

    // Capitalize the message
    for (char * c = message; *c != '\0'; c++) {
      *c = toupper(*c);
    }

    // Send a message to the client
    int rc = send_message(client_socket_fd, message);
    if (rc == -1) {
      perror("Failed to send message to client");
      break;
    }

    // Free the message
    free(message);
  }

  close(client_socket_fd);
  return NULL;
}

int main() {
  // Open a server socket
  unsigned short port = 0;
  int server_socket_fd = server_socket_open(&port);
  if (server_socket_fd == -1) {
    perror("Server socket was not opened");
    exit(EXIT_FAILURE);
  }

  // Start listening for connections, with a maximum of one queued connection
  if (listen(server_socket_fd, 1)) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  while(1) {
    printf("Server listening on port %u\n", port);
  
    // Wait for a client to connect
    int client_socket_fd = server_socket_accept(server_socket_fd);
    if (client_socket_fd == -1) {
      perror("accept failed");
      break;
    }

    printf("Client connected!\n");

    pthread_t thread;

    int * threadarg = malloc(sizeof(int));
    *threadarg = client_socket_fd;

    pthread_create(&thread, NULL, handle_connection, threadarg);    
  }

  // Close sockets
  close(server_socket_fd);

  return 0;
}
