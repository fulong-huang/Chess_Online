#include <server.h>
#include <string>

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

    initServer(portNum);
    std::cout << "Server Started" << std::endl;
    std::thread client_listener(acceptClient);
    std::thread command_processor(processCommands);
    std::string command;
    while(true){
        std::getline(std::cin, command);
        if(command.starts_with("exit")){
            break;
        }
        if(command.starts_with("start")){
            startGame();
        }
        else if(command.starts_with("cooldown")){
            int duration;
            command = command.substr(8);
            try{
                duration = std::stoi(command);
                setCooldownTime(duration);
            }
            catch(std::exception e){
                std::cout << e.what() << std::endl;
            }
        }
        else if(command.starts_with("move")){
            int moves;
            command = command.substr(4);
            try{
                moves = std::stoi(command);
                setMaxMoves(moves);
            }
            catch(std::exception e){
                std::cout << e.what() << std::endl;
            }
        }
        else{
            std::cout << "ELSE" << std::endl;
        }
    }
    std::cout << "SHUTTING DOWN" << std::endl;
    stopServer();
    client_listener.join();
    command_processor.join();
    closeServer();
    std::cout << "SERVER QUIT SUCCESSFULLY" << std::endl;
    return EXIT_SUCCESS;
}

