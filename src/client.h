#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

class Client{

public:
    Client(const char* server_addr, int portNum = 25123);
    
    int send_message(std::string msg);
    int receive_message(char buffer[1024]);
    void close_connection();


private:
    int client_fd;
    struct sockaddr_in serv_addr;
    bool connect_successfully;

    void init_serv_addr(int portNum);
    bool create_socket();
    bool convert_addr(const char* server_addr);
    bool connect_server();

};

