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

  // FILL HINTS to use only IPv4 and TCP
  memset(&hints, 0, sizeof hints); // empty hints 
  hints.ai_family = AF_INET; // only IPv4
  hints.ai_socktype = SOCK_STREAM; // stream sockets (already tcp)

  // LOOKUP
  int status = getaddrinfo(server_ip, server_port, &hints, &servinfo);
  
  // ERROR CHECK
  if (status != 0) {
    fprintf(stderr, "ERROR getaddrinfo - %s\n", gai_strerror(status)); // error to descriptor stderr (2)
    return 1; // show error, not crash
  }
  // entries are already valid

  // iterate linked list
  // I have decided to use a for and not a while so that I don't need to add a connected var
  //  and can just use break. I found it cool
  int socket_descriptor = -1;
  for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {}
    // PRINT //
    struct sockaddr_in *ip_v4 = (struct sockaddr_in *)p->ai_addr;
    // ip buffer
    char ip_buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_v4->sin_addr, ip_buffer, sizeof ip_buffer);
    int port_buffer = ntohs(ip_v4->sin_port);
    // print
    fprintf(stderr, "%s : %s translates to %s : %d\n", server_ip, server_port, ip_buffer, port_buffer);

    // SOCKET //
    // create socket based on servinfo (maybe I should get the node of servinfo that I wnat to connect to)
    
    socket_descriptor =  socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    
    if (socket_descriptor < 0) {
      perror("socket");
      continue; // not crash
    }

    fprintf(stderr, "socket descriptor: %i\n", socket_descriptor);
    
    // CONNECT //
    if (connect(socket_descriptor, p->ai_addr, p->ai_addrlen) < 0) {
      perror("connect");
      close(socket_descriptor);
      socket_descriptor = -2; // to indicate no connection was made
      continue;
    }

    // if we got here it means we made a connection
    break; 
  }

  freeaddrinfo(servinfo); // re-give

  // if we exhausted the linked list
  if (socket_descriptor == -2) {
    fprintf(stderr, "ERROR: A connection was not established");
    return 1;
  }
  
  // SEND //
  // load from stdin by iterating and loading SEND_BUFFER_SIZE bytes at a time
  char stdin_buff[SEND_BUFFER_SIZE];
  while (int len = read(stdin, stdin_buff,  sizeof(stdin_buff)) > 0) {
    int total = 0;

    // until all is sent
    while (total < len) {
      // send byte
      int bytes_sent = send(socket_descriptor, stdin_buff + total, len - total, 0);
      if (bytes_sent == -1) {
        perror("send");
        close(socket_descriptor);
        return 1;
      }

      // look at next byte
      total += bytes_sent;
    }
    if (len < 0) {
      perror("read");
    }
  }
  

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
