#include "server.h"

Server::Server(int portNum){
    this->connect_successfully = true;
    this->init_address(portNum);
    if( !this->create_socket()){
        return;
    }
    if(!this->bind_socket()){
        return;
    }
}


bool Server::success(){
    return this->connect_successfully;
}

bool Server::listen_connection(){
    if(listen(this->server_fd, 1) < 0){
        perror("listen");
        this->connect_successfully = false;
    }
    return this->connect_successfully;
}

bool Server::accept_connection(){
    if((this->client_socket = accept(this->server_fd, 
                    (struct sockaddr*)&this->address, 
                    (socklen_t*)&this->addrlen)) < 0){
        perror("accept");
        this->connect_successfully = false;
    }
    return this->connect_successfully;
}

int Server::send_message(std::string msg){
    return send(this->client_socket, msg.c_str(), msg.size(), 0);
}

int Server::receive_message(char *buffer){
    if(read(this->client_socket, buffer, 1024) <= 0){
        return -1;
    }
    std::cout << this->buffer << std::endl;
    return 1;
}

void Server::shutdown_server(){
    close(this->client_socket);
    shutdown(this->server_fd, SHUT_RDWR);
}



void Server::init_address(int portNum){
    this->addrlen = sizeof(this->address);
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(portNum);
}
bool Server::create_socket(){
    if((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
        this->connect_successfully = false;
    }
    return this->connect_successfully;
}

bool Server::bind_socket(){
    if(bind(this->server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind failed");
        this->connect_successfully = false;
    }
    return this->connect_successfully;
}

void Server::empty_buffer(){
    memset(this->buffer, 0, 1024);
}


