#include <server.h>

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

    client_listener.join();
    command_processor.join();
    return EXIT_SUCCESS;
}

