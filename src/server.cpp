#include <cstdlib>
#include <server.h>

ChessBoard board;

bool server_running;
bool game_running;

int server_fd;
int addrlen;
struct sockaddr_in address;

std::vector<int> client_fd_list;
std::vector<int> teamBlack, teamWhite;

std::map<int, std::string> client_names;

std::mutex client_fd_lock;
std::mutex board_lock;

void splitTeam(){
    teamBlack.clear(); 
    teamWhite.clear();
    std::lock_guard<std::mutex> lock(client_fd_lock);
    std::vector<int> tempClient = client_fd_list;
    bool b;
    while(!tempClient.empty()){
        int randomIdx = std::rand() % tempClient.size();
        if(b){
            teamBlack.push_back(tempClient[randomIdx]);
        }
        else{
            teamWhite.push_back(tempClient[randomIdx]);
        }
        b = !b;
        tempClient.erase(tempClient.begin() + randomIdx);
    }
}

void initServer(int port){
    server_running = true;
    addrlen = sizeof(address);
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "SOCKET ERROR" << std::endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address,
                sizeof(address)) < 0){
        std::cout << "Bind Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(listen(server_fd, 10) < 0){
        std::cout << "Listen Error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void sendBoard(int client_socket){
    std::lock_guard<std::mutex> lock(board_lock);
    sendMessage(client_socket, BOARD_SEND, board.boardToString());
    std::cout << "SEND BOARD" << std::endl;
}

void acceptClient(){
    struct timeval timeout;
    fd_set readSet;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while(server_running){
        int client_socket;
        FD_ZERO(&readSet);
        FD_SET(server_fd, &readSet);
        if(select(server_fd + 1, &readSet, NULL, NULL, &timeout) > 0){
            if((client_socket = accept(server_fd, 
                            (struct sockaddr*)&address, 
                            (socklen_t*)&addrlen)) < 0){
                std::cout << "Failed to accept client: " << client_socket << std::endl;
                std::cout << "Server STOP Listening" << std::endl;
                break;
            }
            else{
                std::cout << "Client Accepted: " << client_socket << std::endl;
                sendMessage(client_socket, CONNECT, "Hello From Server");
                {
                    std::lock_guard<std::mutex> lock(board_lock);
                    sendMessage(client_socket, BOARD_SEND, board.boardToString());
                    std::cout << "SEND BOARD" << std::endl;
                }

                int flags = fcntl(client_socket, F_GETFL, 0);
                if(flags == -1){
                    std::cout << "-1 flags" << std::endl;
                }
                flags |= O_NONBLOCK;
                int result_flagging = fcntl(client_socket, F_SETFL, flags);
                if(result_flagging == -1){
                    std::cout << "ERROR SETTING FLAG" << std::endl;
                }
                {
                    std::lock_guard<std::mutex> lock(client_fd_lock);
                    client_fd_list.push_back(client_socket);
                }
                std::thread client_thread(clientHandler, client_socket);
                client_thread.detach();
            }
        }
    }
    std::cout << "SERVER STOPED" << std::endl;
}

void processCommands(){
    while(server_running){
        int client_fd;
        std::vector<char> msg;
        std::tie(client_fd, msg) = getTopMessage();
        if(msg.size() == 0){
            continue;
        }
        std::string msgString(msg.begin(), msg.end() - 1);
        std::cout << "Received message: " << msgString << ";"<< std::endl;
        
        char promote = QUEEN;
        Msg command_type = (Msg)msg[msg.size()-1];
        if(command_type == MOVE){
            promote = msg[4];
        }
        else if(command_type == BOARD_REQ){
            std::lock_guard<std::mutex> lock(board_lock);
            sendMessage(client_fd, BOARD_SEND, board.boardToString());
            std::cout << "SEND BOARD" << std::endl;
            continue;
        }
        else if(command_type == CONNECT){
            std::cout << "CLIENT CONNECTION MESSAGE" << std::endl;
            continue;
        }
        else if(command_type == SET_NAME){
            client_names[client_fd] = msgString;
        }
        else{
            std::cout << "Invalid message length" << std::endl;
            continue;
        }
        if(!game_running) continue;
        bool move_result, whiteTurn = board.isWhiteTurn();
        {
            std::chrono::steady_clock::time_point curr_time =
                std::chrono::steady_clock::now();
            if(!updateCooldown(curr_time, client_fd)){
                std::cout << "Client " << client_fd << 
                    " attempted to move before cooldown finished" << std::endl;
                std::string cdMessage = getCDAsMessage();
                sendMessage(client_fd, INIT_CD, cdMessage);
                continue;
            }
            move_result = board.move(
                        {msg[0] - '0', msg[1] - '0'}, 
                        {msg[2] - '0', msg[3] - '0'},
                        msg[4]
                    );
        }
        sendMessage(client_fd, RENEW_CD, getPercentage(client_fd));
        if(move_result){
            board.printBoard();
            {
                // disallow new client to connect
                std::lock_guard<std::mutex> lock(client_fd_lock);
                for(int i : client_fd_list){
                    sendMessage(i, MOVE, msgString);
                }
                if(!board.gameIsRunning()){
                    std::lock_guard<std::mutex> lock(board_lock);
                    std::string s;
                    s += !whiteTurn;
                    for(int i : client_fd_list){
                        sendMessage(i, END, s);
                    }
                    board.resetBoard();
                    resetCooldowns();
                }
            }
            std::string s;
        }
        else{
            {
                std::lock_guard<std::mutex> lock(board_lock);
                sendMessage(client_fd, BOARD_SEND, board.boardToString());
                std::cout << "SEND BOARD" << std::endl;
            }
        }
    }
}

void clientHandler(int client_socket){

    while(receiveMessage(client_socket) && server_running){

    }
    close(client_socket);
    return;
}

void initCooldowns(){
    std::chrono::steady_clock::time_point currTime =
        std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(client_fd_lock);
    for(int i : client_fd_list){
        updateCooldown(currTime, i);
    }
}

void stopServer(){
    server_running = false;
}

void closeServer(){
    close(server_fd);
}

void startGame(){
    initCooldowns();
    splitTeam();
    game_running = true;
    board.startGame();
    std::string cdMessage = getCDAsMessage();
    std::lock_guard<std::mutex> lock(board_lock);
    std::string boardString = board.boardToString();
    {
        std::lock_guard<std::mutex> lock(client_fd_lock);
        for(int i : client_fd_list){
            sendMessage(i, BOARD_SEND, boardString);
            sendMessage(i, START, "s");
            sendMessage(i, INIT_CD, cdMessage);
            for(int k : teamWhite){
                sendMessage(i, TEAM_WHITE, client_names[k]);
            }
            for(int k : teamBlack){
                sendMessage(i, TEAM_BLACK, client_names[k]);
            }
        }
    }
}

void endGame(){
    resetCooldowns();
    game_running = false;
}

bool gameIsRunning(){
    return game_running;
}

void updateCD(){
    if(game_running){
        std::string cdMessage = getCDAsMessage();
        std::lock_guard<std::mutex> lock(client_fd_lock);
        for(int i : client_fd_list){
            sendMessage(i, INIT_CD, cdMessage);
            sendMessage(i, RENEW_CD, getPercentage(i));
        }
    }
}


