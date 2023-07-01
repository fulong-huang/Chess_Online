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
        int server_fd;
        std::vector<char> msg;
        std::tie(server_fd, msg) = getTopMessage();
        if(msg.size() == 0){
            continue;
        }
        if(msg.size() == 74){
            game.stringToBoard(msg, msg.size());
            continue;
        }
        if(msg.size() == 1){
            if(msg[0] < 2){
                game.setWinner(!msg[0]);
                continue;
            }
            if(msg[0] == 's'){
                game.startGame();
                continue;
            }
            std::cout << "===== UNKNOWN COMMAND ======" << std::endl;
            continue;
        }
        if(msg.size() == 2){
            game.updateServerCD(msg[0]);
            continue;
        }
        if(msg.size() == 3){
            game.initCD(msg);
            continue;
        }
        if(msg.size() == 5){
            bool moveResult = game.move(
                        {msg[0] - '0', msg[1] - '0'}, 
                        {msg[2] - '0', msg[3] - '0'}
                    );
            if(!moveResult){
                sendMessage(client_fd, BOARD_REQ, "b");
            }
        }
        std::string msgString(msg.begin(), msg.end());
        std::cout << "RECEIVED MESSAGE: " << msgString << std::endl;
    }
}

void runGame(){
    std::chrono::steady_clock::time_point prev_time, curr_time;
    prev_time = std::chrono::steady_clock::now();
    while(gameRunning){
        // Update times
        curr_time = std::chrono::steady_clock::now();
        game.updateCurrCD(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        curr_time - prev_time
                    ).count());
        prev_time = curr_time;
        while(game.window.pollEvent(game.event)){
            switch(game.event.type){
                case sf::Event::Closed:
                    {
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
                            std::string msg = 
                                combinePositions(game.savedFrom, game.savedTo);
                            sendMessage(client_fd, MOVE, msg);
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


