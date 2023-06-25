#include "client.h"

Client::Client(const char* server_addr, int portNum){

    this->init_serv_addr(portNum);

    if(!this->create_socket() ||
            !this->convert_addr(server_addr) ||
            !this->connect_server()){
        return;
    }

    return;
}

void Client::init_serv_addr(int portNum){
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_port = htons(portNum);
}

bool Client::create_socket(){
    if((this->client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return false;
    }
    return true;
}

bool Client::convert_addr(const char* server_addr){
    if(inet_pton(AF_INET, server_addr, &this->serv_addr.sin_addr) <= 0){
        printf("\nInvalid address/ Address not supported \n");
        return false;
    }
    return true;
}

bool Client::connect_server(){
    if(connect(this->client_fd, (struct sockaddr*)&this->serv_addr,
               sizeof(this->serv_addr))
            < 0){
        printf("\nConnection Failed \n");
        return false;
    }
    return true;
}

int Client::send_message(std::string msg){
    return send(this->client_fd, msg.c_str(), msg.size(), 0);
}

int Client::receive_message(char buffer[1024]){
    int result = read(this->client_fd, buffer, 1024);
    return result;
}

void Client::close_connection(){
    close(client_fd);
}



