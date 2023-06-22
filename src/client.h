#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

class Client{

public:
    Client(const char* server_addr, int portNum = 25123);
    
    int send_message(std::string msg);
    int receive_message();
    void close_connection();
    void empty_buffer();


private:
    int client_fd;
    char buffer[1024];
    struct sockaddr_in serv_addr;
    bool connect_successfully;

    void init_serv_addr(int portNum);
    bool create_socket();
    bool convert_addr(const char* server_addr);
    bool connect_server();

};

