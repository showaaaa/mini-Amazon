#include "socket.h"

#include <vector>
#include "exception.h"

int open_listenfd(const char * port){
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    
    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        //cerr << "Error: cannot get address info for host" << endl;
        //cerr << "  (" << hostname << "," << port << ")" << endl;
        //return -1;
        throw Failure("cannot get address info for host");
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        // cerr << "Error: cannot create socket" << endl;
        // cerr << "  (" << hostname << "," << port << ")" << endl;
        // return -1;
        throw Failure("cannot create socket");
    } //if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        // cerr << "Error: cannot bind socket" << endl;
        // cerr << "  (" << hostname << "," << port << ")" << endl;
        // return -1;
        throw Failure("cannot bind socket");
    } //if

    status = listen(socket_fd, 100);
    if (status == -1) {
        // cerr << "Error: cannot listen on socket" << endl; 
        // cerr << "  (" << hostname << "," << port << ")" << endl;
        // return -1;
        throw Failure("cannot listen on socket");
    } //if

    freeaddrinfo(host_info_list);
    return socket_fd;
}

int open_clientfd(const char * hostname, const char * port){
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        //cerr << "Error: cannot get address info for host" << endl;
        //cerr << "  (" << hostname << "," << port << ")" << endl;
        //return -1;
        throw Failure("cannot get address info for host");
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        //cerr << "Error: cannot create socket" << endl;
        //cerr << "  (" << hostname << "," << port << ")" << endl;
        //return -1;
        throw Failure("cannot create socket");
    } //if
    
    cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
    
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        //cerr << "Error: cannot connect to socket" << endl;
        //cerr << "  (" << hostname << "," << port << ")" << endl;
        //return -1;
        throw Failure("cannot connect socket");
    } //if
    freeaddrinfo(host_info_list);
    return socket_fd;
}

int accept_getip(int socket_fd, string * ip){
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
        // cerr << "Error: cannot accept connection on socket" << endl;
        // return -1;
        throw Failure("cannot accept connection on socket");
    } //if
    
    struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;
    *ip = inet_ntoa(addr->sin_addr);

    return client_connection_fd;
}

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
void sendMsg(int socket_fd, const void * buf, int len) {
  if (send(socket_fd, buf, len, 0) < 0) {
    //close(socket_fd);
    throw Failure("fail to send msg.");
  }
}

/*
  receive msg from the given socket only once.
  If fails, it will throw exception and close socket.
*/
string recvMsg(int socket_fd) {
  vector<char> buffer(MAX_LENGTH, 0);

  int len = recv(socket_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
  if (len <= 0) {
    //close(socket_fd);
    cerr << "len: " << len << endl;
    cerr << "errno: " << errno << endl;
    throw Failure("fail to receive message.");
  }

  string msg(buffer.data(), len);
  return msg;
}
