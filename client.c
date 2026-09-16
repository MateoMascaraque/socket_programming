#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

// added for printing, debugging and testing
#include <arpa/inet.h>      // inet_ntop, ntohs

#define SEND_BUFFER_SIZE 2048


/* TODO: client()
 * Open socket and send message from stdin.
 * Return 0 on success, non-zero on failure
*/
int client(char *server_ip, char *server_port) {
  // GET ADDRES INFO //
  struct addrinfo hints; // info about our connection
  struct addrinfo *servinfo; // pointer to results given by getaddrinfo()

  // set hints to use only IPv4 and TCP
  memset(&hints, 0, sizeof hints); // empty hints 
  hints.ai_family = AF_INET; // only IPv4
  hints.ai_socktype = SOCK_STREAM; // stream sockets (already tcp)

  // get address information (pointers to hints and servinfo)
  int status = getaddrinfo(server_ip, server_port, &hints /*IPv4*/, &servinfo /*lnkedlist to store info*/);
  
  // error check
  if (status != 0) {
    fprintf(stderr, "ERROR getaddrinfo - %s\n", gai_strerror(status)); // error to descriptor stderr (2)
    return 1; // show error, not crash
  }

  // print the resolved pages
  // iterate linked list
  for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
    struct sockaddr_in *ip_v4 = (struct sockaddr_in *)p->ai_addr;

    // now to print the ip nad port
    // save in a buffer
    char ip_buffer[16];
    // network to presentation
    inet_ntop(AF_INET, &ip_v4->sin_addr, ip_buffer, sizeof ip_buffer);
    // network to host
    int port_buffer = ntohs(ip_v4->sin_port);

    fprintf(stderr, "%s : %s translates to %s : %d\n", server_ip, server_port, ip_buffer, port_buffer);
  }
  
  // socket
  
  // connect
  
  // send / recv
  freeaddrinfo(servinfo);
  return 0;
}

/*
 * main()
 * Parse command-line arguments and call client function
*/
int main(int argc, char **argv) {
  char *server_ip;
  char *server_port;

  if (argc != 3) {
    fprintf(stderr, "Usage: ./client-c [server IP] [server port] < [message]\n");
    exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  return client(server_ip, server_port);
}
