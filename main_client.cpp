#include <cstdlib>
#include <iostream>
#include <client.h>
#include <thread>

int main(int argc, const char* argv[]){
    if(argc != 3){
        std::cout << "Invalid number of arguments, \n"
            "\tExpected [Server IP address] and [port]" << std::endl;
        return EXIT_FAILURE;
    }

    int port;
    try{
        port = std::stoi(argv[2]);
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initClient(argv[1], port);
    messageSender(CONNECT, "Hello From Client");

    std::thread message_receiver(messageReceiver);
    std::thread command_processor(processCommands);

    runGame();

    message_receiver.join();
    command_processor.join();

    return 0;
}

