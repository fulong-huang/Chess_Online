#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 
#include <message.h>

std::mutex queue_lock;
std::queue<std::pair<int, std::vector<char>>> message_queue;

void sendMessage(int fd, Msg messageType, std::string message){
    char buffer[message.size() + 1];
    buffer[0] = messageType;
    message.copy(buffer + 1, message.size());
    send(fd, buffer, message.size() + 1, 0);
}

bool receiveMessage(int fd){
    char buffer[128];
    int byteRead = read(fd, buffer, 1);
    if(byteRead == -1){
        return true;
    }
    if(byteRead == 0){
        std::cout << "Client Disconnected" << std::endl;
        return false;
    }
    switch(buffer[0]){
        case CONNECT:
            byteRead = 17;
            break;
        case MOVE:
            byteRead = 5;
            break;
        case BOARD_SEND:
            byteRead = 74;
            break;
        case BOARD_REQ:
            byteRead = 1;
            break;
        case START:
            byteRead = 1;
            break;
        case END:
            byteRead = 1;
            break;
        case INIT_CD:
            byteRead = 3;
            break;
        case RENEW_CD:
            byteRead = 2;
            break;
        default:
            byteRead = 128;
            std::cout << "======UNKNOWN COMMAND======" << std::endl;
            std::cout << "======Clearing Pending Messages======" << std::endl;
            while(byteRead == 128){
                byteRead = read(fd, buffer, 128);
            }
            return true;
    }
    byteRead = read(fd, buffer, byteRead);
    if(byteRead <= 0){
        std::cout << "========= UNKNOWN ERROR =========" << std::endl;
        return false;
    }
    buffer[byteRead] = 0;

    std::vector<char> data(buffer, buffer + byteRead);
    std::lock_guard<std::mutex> lock(queue_lock);
    message_queue.push({fd, data});
    return true;
}

std::pair<int, std::vector<char>> getTopMessage(){
    if(message_queue.empty()){
        return {-1, {}};
    }
    std::pair<int, std::vector<char>> msg = message_queue.front();
    message_queue.pop();
    return msg;
}


