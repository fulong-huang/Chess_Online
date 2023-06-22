#include <iostream>
#include <client.h>

int main(int argc, const char* argv[]){
    std::cout << "Client Started" << std::endl;

    Client c(argv[1]);

    c.send_message("Hello from Client!!!!!!!!!!");
    c.receive_message();

    while(true){
        if(c.receive_message() < 0){
            std::cout << "Server Exited" << std::endl;
            break;
        }
    }

    c.close_connection();

    return 0;
}

