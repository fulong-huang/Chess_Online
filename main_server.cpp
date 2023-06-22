#include <cstdlib>
#include <iostream>
#include <thread>
#include "board.h"
#include "game.h"
#include "server.h"

// VERY BAD!!!
void communicate( ChessBoard* board, Game* game){

    Server s;
    if(!s.success() || 
            !s.listen_connection() ||
            !s.accept_connection()){
        return ;
    }
    s.send_message("Hello from Server");
    char buffer[1024];
    s.receive_message(buffer);


    while(true){
        if(s.receive_message(buffer) < 0){
            std::cout << "Disconnected" << std::endl;
            break;
        }

        if(strcmp(buffer, "exit") == 0){
            std::cout << "Received exit, exiting" << std::endl;
            break;
        }
        int result = board->move({buffer[0]-'0', buffer[1]-'0'},{buffer[2]-'0', buffer[3]-'0'}); 
        game->updateBoard();
    }

    s.shutdown_server();
    std::cout << "EXIT SUCCESSFULLY" << std::endl;
    
}

int main(int argc, char* argv[]){
    ChessBoard board;
    Game g(&board);
    std::thread comm(communicate, &board, &g);
    int count = 0;
    while(g.isRunning()){
        g.update();
        g.display();
        count++;
        if(count > 50){
            count = 0;
            board.printBoard();
        }
    }
    comm.join();

    return EXIT_SUCCESS;
}

