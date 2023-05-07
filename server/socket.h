#ifndef _SOCKET_H
#define _SOCKET_H

#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
//#include "exception.h"
//#include "server.h"
//#define MAXLINE 65536

const int MAX_LENGTH = 65536;

using namespace std;
int open_listenfd(const char * port);

int open_clientfd(const char * hostname, const char * port);

int accept_getip(int socket_fd, string * ip);

// int getport(int socket_fd){
//     struct sockaddr_in sin;
//     socklen_t len = sizeof(sin);
//     if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) == -1) {
//         cerr << "Error: cannot getsockname" << endl;
//         exit(EXIT_FAILURE);
//     }
//     return ntohs(sin.sin_port);
// }

/*
  send msg to the given socket only once. If fails, 
  it will throw exception and close socket.
*/
void sendMsg(int socket_fd, const void * buf, int len);

/*
  receive msg from the given socket only once.
  If fails, it will throw exception and close socket.
*/
string recvMsg(int socket_fd);
#endif