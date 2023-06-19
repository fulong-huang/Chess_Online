#include <cstdlib>
#include <iostream>
#include "game.h"

int main(int argc, char* argv[]){

    Game game;
    while(game.isRunning()){
        game.update();
        game.display();
    }

    return EXIT_SUCCESS;
}

