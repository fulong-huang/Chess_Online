#include <server.h>

ChessBoard board;

int server_fd;
int addrlen;
struct sockaddr_in address;

std::vector<int> client_fd_list;

std::mutex client_fd_lock;
std::mutex board_lock;

void initServer(int port){
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

void acceptClient(){
    struct timeval timeout;
    fd_set readSet;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while(true){
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
    while(true){
        int client_fd;
        std::vector<char> msg;
        std::tie(client_fd, msg) = getTopMessage();
        if(msg.size() == 0){
            continue;
        }
        std::string msgString(msg.begin(), msg.end());
        std::cout << "Received message: " << msgString << std::endl;
        
        char promote = QUEEN;
        if(msg.size() == 5){
            promote = msg[4];
        }
        else{
            std::cout << "Invalid message length" << std::endl;
            continue;
        }
        bool move_result;
        {
            std::chrono::steady_clock::time_point curr_time =
                std::chrono::steady_clock::now();
            if(!updateCooldown(curr_time, client_fd)){
                std::cout << "Client " << client_fd << 
                    " attempted to move before cooldown finished" << std::endl;
                continue;
            }
            move_result = board.move(
                        {msg[0] - '0', msg[1] - '0'}, 
                        {msg[2] - '0', msg[3] - '0'}
                    );
            std::cout << "Client move result: " << move_result << std::endl;
            if(!board.gameIsRunning()){
                board.resetBoard();
                resetCooldowns();
            }
        }
        if(move_result){
            board.printBoard();
            {
                std::lock_guard<std::mutex> lock(client_fd_lock);
                for(int i : client_fd_list){
                    sendMessage(i, MOVE, msgString);
                }
            }
        }
        else{
            // TODO:
            //  Send board to current fd
            {
                std::lock_guard<std::mutex> lock(board_lock);
                sendMessage(client_fd, BOARD_SEND, board.boardToString());
                std::cout << "SEND BOARD" << std::endl;
            }
        }
    }
}

void clientHandler(int client_socket){

    while(receiveMessage(client_socket)){

    }
    close(client_socket);
    return;
}

//void initCooldowns(){
//    std::chrono::steady_clock::time_point currTime =
//        std::chrono::steady_clock::now();
//    for(int i : client_fd_list){
//        updateCooldown(currTime, i);
//    }
//}


