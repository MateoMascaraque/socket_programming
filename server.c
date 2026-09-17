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

  // copied from client, replace connect with setsockopt and bind
  int socket_descriptor = -1;
  for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
    // SOCKET //
    // create socket based on servinfo (maybe I should get the node of servinfo that I wnat to connect to)
    
    socket_descriptor =  socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    
    if (socket_descriptor < 0) {
      perror("socket");
      continue; // not crash
    }

    

    // if we got here it means we made a connection
    break; 
  }

  freeaddrinfo(servinfo); // re-give




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
