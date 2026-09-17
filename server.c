#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048

/* TODO: server()
 * Open socket and wait for client to connect
 * Print received message to stdout
 * Return 0 on success, non-zero on failure
*/
int server(char *server_port) {
  // GET ADDRES INFO //
  struct addrinfo hints; // connection info
  struct addrinfo *servinfo; // pointer to server info

  // FILL HINTS to use only IPv4 and TCP (same as client)
  memset(&hints, 0, sizeof hints); // empty hints 
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_flags = AI_PASSIVE; // fill in my address

  int status = getaddrinfo(NULL, server_port, &hints, &servinfo); // null as filled on its own
  if (status != 0) {
    fprintf(stderr, "ERROR S getaddrinfo - %s\n", gai_strerror(status));
    return 1;
  }

  // ITERATE THROUGH SERVINFO
  // copied from client, replace connect with setsockopt and bind
  int socket_descriptor = -1;
  for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
    // SOCKET //
    // create socket based on servinfo
    
    socket_descriptor =  socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    
    if (socket_descriptor < 0) {
      perror("socket");
      continue; // not crash
    }

    // SETSOCKOPT //
    // So that I dont get the already in use error
    int optval = 1;
    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) < 0) {
      perror("setsockopt");
      close(socket_descriptor);
      socket_descriptor = -1;
      continue;
    }

    // BIND //
    // almost like connect in client
    if (bind(socket_descriptor, p->ai_addr, p->ai_addrlen) < 0) {
      perror("bind");
      close(socket_descriptor);
      socket_descriptor = -1; // to indicate bind was unsuccessful
      continue;
    }

    // if we got here it means we made a bind
    break; 
  }

  freeaddrinfo(servinfo); // re-give
  
  // if we exhausted the linked list
  if (socket_descriptor == -1) {
    fprintf(stderr, "ERROR: could not bind to port %s\n", server_port);
    return 1;
  }

  // LISTEN //
  if (listen(socket_descriptor, QUEUE_LENGTH) < 0) {
    perror("listen");
    close(socket_descriptor);
    return 1;
  }

  // ACCEPT //
  // accepting is going to give us a different file descriptor so that we can open connections while still listening
  char recv_buff[RECV_BUFFER_SIZE];
  ssize_t len;
  int client_descriptor;

  // loop to accept
  while (true) {
    client_descriptor = accept(sockfd,NULL,NULL); // we don't need a requirement
    if (client_descriptor == -1) {
        perror("accept");
        continue; // not block
    }

    // receive in peices (like in clients)
    while ((len = recv(client_descriptor, recv_buff, sizeof recv_buff, 0)) > 0) {
      fwrite(recv_buff, 1, len, stdout);
    }
    if (len < 0) {
      perror("recv");
    }
 
    fflush(stdout); // output to stdout
    close(client_descriptor);  // clear the client
  }
  
  close(socket_descriptor);
  return 0;
}

/*
 * main():
 * Parse command-line arguments and call server function
*/
int main(int argc, char **argv) {
  char *server_port;

  if (argc != 2) {
    fprintf(stderr, "Usage: ./server-c [server port]\n");
    exit(EXIT_FAILURE);
  }

  server_port = argv[1];
  return server(server_port);
}
