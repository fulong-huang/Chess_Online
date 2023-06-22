#include <cstdlib>
#include <iostream>
#include "game.h"
#include "server.h"

int main(int argc, char* argv[]){

    Server s;
    if(!s.success() || 
            !s.listen_connection() ||
            !s.accept_connection()){
        return EXIT_FAILURE;
    }
    s.send_message("Hello from Server");
    s.receive_message();


    char buffer[1024];
    while(true){
        std::cout << "Enter a message to send ('exit' to exit): ";
        std::cin.getline(buffer, sizeof(buffer));
        
        if(s.send_message(buffer) < 0){
            std::cout << "failed to send data to client." << std::endl;
            break;
        }

        if(strcmp(buffer, "exit") == 0){
            std::cout << "Server is exiting." << std::endl;
            break;
        }
    }

    s.shutdown_server();
    std::cout << "EXIT SUCCESSFULLY" << std::endl;
//    Game game;
//    while(game.isRunning()){
//        game.update();
//        game.display();
//    }

    return EXIT_SUCCESS;
}

