#include <iostream>
#include <client.h>
#include <game.h>
#include <thread>
#include <fcntl.h>

Game game;
std::atomic<bool> gameRunning(true);

void client_send(Client* c, std::string msg){
    c->send_message(msg);
}

void client_thread(Client* c){
    std::cout << "Client Started" << std::endl;
    char buffer[1024];
    int byteRead = c->receive_message(buffer);
    buffer[byteRead] = 0;
    std::cout << "Received: " << std::endl;
    std::cout << buffer << std::endl;
    byteRead = c->receive_message(buffer);
    std::cout << "Received board: "  << std::endl;
    std::cout << buffer << std::endl;
    game.stringToBoard(buffer, byteRead);

    int flags = fcntl(c->client_fd, F_GETFL, 0);
    if(flags == -1){
        std::cout << "-1 flags" << std::endl;
    }
    flags |= O_NONBLOCK;
    int result_flagging = fcntl(c->client_fd, F_SETFL, flags);
    if(result_flagging == -1){
        std::cout << "ERROR SETTING FLAG" << std::endl;
    }
    while(gameRunning){
        int byteRead = c->receive_message(buffer);
        buffer[byteRead] = 0;
        if(byteRead < 0){
            continue;
        }
        if(byteRead == 0){
            std::cout << "SERVER CONNECTION FAILED" << std::endl;
            gameRunning = false;
            break;
        }
        std::cout << "RECEIVED: " << buffer << std::endl;
        if(byteRead == 4){
            std::cout << "4 Bytes Read" << std::endl;
            bool move_result = game.move({buffer[0]-'0', buffer[1]-'0'}, 
                    {buffer[2]-'0', buffer[3]-'0'});
            std::cout << "MOVE RESULT: " << move_result << std::endl;
        }
//        std::cout << "Enter a message to send ('exit' to exit): ";
//        std::cin.getline(buffer, sizeof(buffer));
//        
//        if(c->send_message(buffer) < 0){
//            std::cout << "failed to send data to client." << std::endl;
//            break;
//        }
//
//        if(strcmp(buffer, "exit") == 0){
//            std::cout << "Server is exiting." << std::endl;
//            break;
//        }
    }

    c->close_connection();
}

std::string combinePositions(std::pair<int, int> from, std::pair<int, int> to){
    std::string result;
    result += (from.first + '0');
    result += (from.second + '0');
    result += (to.first + '0');
    result += (to.second + '0');

    return result;
}

int main(int argc, const char* argv[]){

    bool running = true;
    Client c(argv[1]);
    std::thread client(client_thread, &c);
    while(gameRunning){
        while(game.window.pollEvent(game.event)){
            switch(game.event.type){
                case sf::Event::Closed:
                    {
                        running = false;
                        gameRunning = false;
                        break;
                    }
                case sf::Event::Resized:
                    {
                        game.resizeBoard();
                        break;
                    }
                case sf::Event::MouseButtonPressed:
                    {
                        bool success = game.handleMouseClick();
                        if(success){
                            std::cout << "SUCCESS" << std::endl;
                            std::string msg = combinePositions(game.savedFrom, game.savedTo);
                            std::thread client_send_thread(client_send, &c, msg);
                            client_send_thread.detach();
                            game.moveFrom.first = -1;
                        }
                    }
                default:
                    break;
            }
        }
        game.display();
    }
    client.join();
    c.close_connection();
    return 0;
}

