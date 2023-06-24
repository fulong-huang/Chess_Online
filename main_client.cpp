#include <iostream>
#include <client.h>

int main(int argc, const char* argv[]){
    std::cout << "Client Started" << std::endl;

    Client c(argv[1]);

    char buffer[1024];
    c.send_message("Hello from Client!!!!!!!!!!");
    c.receive_message(buffer);

    while(true){
        std::cout << "Enter a message to send ('exit' to exit): ";
        std::cin.getline(buffer, sizeof(buffer));
        
        if(c.send_message(buffer) < 0){
            std::cout << "failed to send data to client." << std::endl;
            break;
        }

        if(strcmp(buffer, "exit") == 0){
            std::cout << "Server is exiting." << std::endl;
            break;
        }
    }

    c.close_connection();

    return 0;
}

