
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

class Server{
public:
    Server(int portNum = 25123);

    bool success();

    bool listen_connection();

    bool accept_connection();

    void send_message();

    void receive_message();
    
    void shutdown_server();


private:
    int server_fd, client_socket;
    bool connect_successfully;
    struct sockaddr_in address;
    int addrlen;


    void init_address(int portNum);
    bool create_socket();
    bool bind_socket();
};

