#include "client.h"

sf::TcpSocket client_socket;
std::string client_name;
//struct sockaddr_in serv_addr;

Game game;
std::atomic<bool> gameRunning(true);

void initClient(const char* server_addr, int port, const char* name){
    client_name = name;
	sf::Socket::Status status = client_socket.connect(server_addr, port);
	if(status != sf::Socket::Done){
		std::cout << "SOCKET CONNECTION ERROR" << std::endl;
		exit(EXIT_FAILURE);
	}
	game.player_name = client_name;
	sendMessage(&client_socket, SET_NAME, client_name);
//	char data[4] = {SET_NAME, 10, 'F', 'i'};
//	std::cout << "CLIENT NAME: " << client_name << std::endl;
//	if(client_socket.send(data, 4) != sf::Socket::Done){
//		std::cout << "SOCKET SEND ERROR" << std::endl;
//		exit(EXIT_FAILURE);
//	}
 //   serv_addr.sin_family = AF_INET;
 //   serv_addr.sin_port = htons(port);
 //   if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
 //       std::cout << "SOCKET Creation Error" << std::endl;
 //       exit(EXIT_FAILURE);
 //   }
 //   if(inet_pton(AF_INET, server_addr, &serv_addr.sin_addr) <= 0){
 //       std::cout << "Invalid address " << std::endl;
 //       exit(EXIT_FAILURE);
 //   }
 //   if(connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
 //       std::cout << "Connection Failed" << std::endl;
 //       exit(EXIT_FAILURE);
 //   }
 //   
 //   int flags = fcntl(client_fd, F_GETFL, 0);
 //   if(flags == -1){
 //       std::cout << "-1 flags" << std::endl;
 //   }
 //   flags |= O_NONBLOCK;
 //   int result_flagging = fcntl(client_fd, F_SETFL, flags);
 //   if(result_flagging == -1){
 //       std::cout << "ERROR SETTING FLAG" << std::endl;
 //   }
 //   if(client_name.size() > 10){
 //       client_name = client_name.substr(0, 10);
 //   }
 //   game.player_name = client_name;
 //   std::cout << "CLIENT NAME: " << client_name << std::endl;
 //   sendMessage(client_fd, SET_NAME, client_name);
}

void messageReceiver(){
    while(receiveMessage(&client_socket) && gameRunning){
    }
    std::cout << "MESSAGE RECEIVER ENDED" << std::endl;
    // close(client_fd);
    gameRunning = false;
}

void messageSender(Msg messageType, std::string msg){
    sendMessage(&client_socket, messageType, msg);
}

void processCommands(){
    while(gameRunning){
        int server_fd;
        std::vector<char> msg;
        std::tie(server_fd, msg) = getTopMessage();
        if(msg.size() == 0){
            continue;
        }
        Msg command_type = (Msg)msg[msg.size()-1];
        msg.erase(msg.end() - 1);
        if(command_type == BOARD_SEND){
            game.stringToBoard(msg, msg.size());
            continue;
        }
        if(command_type == END){
            std::cout << "Winner: " << (int) msg[0] << std::endl;
            game.setWinner(!msg[0]);
            continue;
        }
        if(command_type == START){
            game.startGame();
            continue;
        }
        if(command_type == RENEW_CD){
            game.updateServerCD(msg[0]);
            continue;
        }
        if(command_type == INIT_CD){
            game.initCD(msg);
            continue;
        }
        if(command_type == MOVE){
            bool moveResult = game.move(
                        {msg[0] - '0', msg[1] - '0'}, 
                        {msg[2] - '0', msg[3] - '0'},
                        msg[4]
                    );
            if(!moveResult){
                sendMessage(&client_socket, BOARD_REQ, "b");
            }
        }
        if(command_type == TEAM_WHITE){
            std::string message_string(msg.begin(), msg.end());
            game.addWhiteTeam(message_string);
            continue;
        }
        if(command_type == TEAM_BLACK){
            std::string message_string(msg.begin(), msg.end());
            game.addBlackTeam(message_string);
            continue;
        }
        std::string msgString(msg.begin(), msg.end());
        std::cout << "RECEIVED MESSAGE: " << msgString << std::endl;
    }
}

void runGame(){
    std::chrono::steady_clock::time_point prev_time, curr_time;
    prev_time = std::chrono::steady_clock::now();

//    int flag = fcntl(0, F_GETFL, 0);
//    fcntl(0, F_SETFL, flag | O_NONBLOCK);
    char commandRead[128];
    
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
                                combinePositions(game.savedFrom,
                                        game.savedTo, 
                                        game.selected_promotion);
                            sendMessage(&client_socket, MOVE, msg);
                            game.moveFrom.first = -1;
                        }
                    }
                default:
                    break;
            }
        }
//        int readLen = read(0, commandRead, 50);
//        if(readLen > 0){
//            std::cout << "LEN " << std::endl;
//            if(readLen == 2 && commandRead[0] == 'b'){
//                sendMessage(&client_socket, BOARD_REQ, "b");
//            }
//            else if(readLen == 5){
//                commandRead[4] = 0;
//                sendMessage(&client_socket, MOVE, std::string(commandRead, 5));
//            }
//            else if(readLen == 6){
//                commandRead[4] -= '0';
//                sendMessage(&client_socket, MOVE, std::string(commandRead, 5));
//            }
//            else{
//                std::cout << "INVALID COMMAND" << std::endl;
//            }
//        }
//
        game.display();
    }
    std::cout << "Game end" << std::endl;

}

std::string combinePositions(std::pair<int, int> from,
        std::pair<int, int> to,
        int piece){
    std::string result;
    result += (from.first + '0');
    result += (from.second + '0');
    result += (to.first + '0');
    result += (to.second + '0');

    result += (5 - piece);

    return result;
}


