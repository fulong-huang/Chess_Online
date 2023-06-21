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
    s.send_message();
    s.receive_message();
    s.shutdown_server();
    std::cout << "EXIT SUCCESSFULLY" << std::endl;
    Game game;
    while(game.isRunning()){
        game.update();
        game.display();
    }

    return EXIT_SUCCESS;
}

