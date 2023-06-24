#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <atomic>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <fcntl.h>


std::vector<int> client_sockets;
std::mutex client_sockets_lock;

std::atomic<bool> listenFlag(false);

void handleClient(int client_socket);

int server_init(int server_port, struct sockaddr_in address){
    int server_fd;
    int addrlen = sizeof(address);
    //
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        // Handle error
    }
  
    // Forcefully attaching socket to the port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void server_listen(int server_fd, struct sockaddr_in* address){
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int addrLen = sizeof(&address);
    struct timeval timeout;
    fd_set readSet;
    timeout.tv_sec = 1;
    while(!listenFlag){
        int new_socket;
        FD_ZERO(&readSet);
        FD_SET(server_fd, &readSet);
        std::cout << "Checking pending connections" << std::endl;
        if(select(server_fd + 1, &readSet, NULL, NULL, &timeout) > 0){
            std::cout << "PENDING CONNECTION" << std::endl;
            if((new_socket = accept(server_fd, (struct sockaddr*)&address, 
                            (socklen_t*)&addrLen)) < 0){
                std::cout << "Failed to accept client" << std::endl;
                return;
            }
            else{
                std::cout << "Client accepted" << std::endl;
                std::thread client_thread(handleClient, new_socket);
                client_thread.detach();
                {
                    std::lock_guard<std::mutex> lock(client_sockets_lock);
                    client_sockets.push_back(new_socket);
                }
                std::cout << "Thread Detached" << std::endl;
            }
        }
    }
}

void handleClient(int client_socket){
    std::string msg = "Hello from server";
    send(client_socket, msg.c_str(), msg.size(), 0);
    char buffer[1024];

    int flags = fcntl(client_socket, F_GETFL, 0);
    if(flags == -1){
        std::cout << "-1 flags" << std::endl;
    }
    flags |= O_NONBLOCK;
    int result_flagging = fcntl(client_socket, F_SETFL, flags);
    if(result_flagging == -1){
        std::cout << "ERROR SETTING FLAG" << std::endl;
    }
    while(!listenFlag){
        memset(buffer, 0, 1024);
        int byteRead = read(client_socket, buffer, 1024);
        if(byteRead == -1){
            continue;
        }
        std::cout << "Received message from client " << client_socket << 
            ": " << buffer << std::endl;
        {   // Mutex lock
            std::lock_guard<std::mutex> lock(client_sockets_lock);
            for(int i : client_sockets){
                if(i != client_socket){
                    send(i, buffer, byteRead, 0);
                }
            }
        }
        
        if(byteRead == 0){
            std::cout << "Client Disconnect" << std::endl;
            break;
        }
        if(strcmp(buffer, "exit") == 0){
            std::cout << "Client Exit" << std::endl;
            break;
        }
    }
    {   // Mutex Lock
        std::lock_guard<std::mutex> lock(client_sockets_lock);
        auto it = std::find(
                client_sockets.begin(), 
                client_sockets.end(), 
                client_socket);
        if(it != client_sockets.end()){
            client_sockets.erase(it);
        }
    }
    close(client_socket);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cout << 
            "Invalid number of arguments, exptect port number" << 
            std::endl;
        return -1;
    }
    int portNum;
    try{
        portNum = std::stoi(argv[1]);   
    }
    catch(const std::exception &e){
        std::cout << "Exception: " << e.what() << std::endl;
        return -1;
    }


    struct sockaddr_in address;
    int server_fd = server_init(portNum, address);

    std::thread listen_thread(server_listen, server_fd, &address);

    std::cout << "Press Enter to stop accepting client" << std::endl;
    std::cin.ignore();
    listenFlag = true;
    listen_thread.join();
    std::cout << "Press Enter to EXIT" << std::endl;
    std::cin.ignore();
    return EXIT_SUCCESS;
}

