#include "client.h"

int client_fd;
struct sockaddr_in serv_addr;

Game game;
std::atomic<bool> gameRunning(true);

void initClient(const char* server_addr, int port){
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "SOCKET Creation Error" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(inet_pton(AF_INET, server_addr, &serv_addr.sin_addr) <= 0){
        std::cout << "Invalid address " << std::endl;
        exit(EXIT_FAILURE);
    }
    if(connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        std::cout << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    int flags = fcntl(client_fd, F_GETFL, 0);
    if(flags == -1){
        std::cout << "-1 flags" << std::endl;
    }
    flags |= O_NONBLOCK;
    int result_flagging = fcntl(client_fd, F_SETFL, flags);
    if(result_flagging == -1){
        std::cout << "ERROR SETTING FLAG" << std::endl;
    }
}

void messageReceiver(){
    while(receiveMessage(client_fd) && gameRunning){
    }
    std::cout << "MESSAGE RECEIVER ENDED" << std::endl;
    close(client_fd);
    gameRunning = false;
}

void messageSender(Msg messageType, std::string msg){
    sendMessage(client_fd, messageType, msg);
}

void processCommands(){
    while(gameRunning){
        std::vector<char> msg = getTopMessage();
        if(msg.size() == 0){
            continue;
        }
        if(msg.size() == 74){
            game.stringToBoard(msg, msg.size());
            continue;
        }
        if(msg.size() == 5){
            bool moveResult = game.move(
                        {msg[0] - '0', msg[1] - '0'}, 
                        {msg[2] - '0', msg[3] - '0'}
                    );
            // TODO:
            //  if move failed, request for new board
        }
        std::string msgString(msg.begin(), msg.end());
        std::cout << "RECEIVED MESSAGE: " << msgString << std::endl;
    }
}

void runGame(){
    bool running = true;
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
                            // TODO
                            //  Add a field to get promote unit
                            std::string msg = 
                                combinePositions(game.savedFrom, game.savedTo);
                            messageSender(Msg_Move, msg);
                            game.moveFrom.first = -1;
                        }
                    }
                default:
                    break;
            }
        }
        game.display();
    }
    std::cout << "Game end" << std::endl;

}

std::string combinePositions(std::pair<int, int> from, std::pair<int, int> to){
    std::string result;
    result += (from.first + '0');
    result += (from.second + '0');
    result += (to.first + '0');
    result += (to.second + '0');
    result += QUEEN;

    return result;
}


