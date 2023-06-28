#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 

enum Msg{
    Msg_Connect,
    Msg_Board,
    Msg_Move,
    
};

std::mutex queue_lock;
std::queue<std::vector<char>> message_queue;

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
        case Msg_Connect:
            std::cout << "CONNECT" << std::endl;
            byteRead = 17;
            break;
        case Msg_Move:
            std::cout << "Move" << std::endl;
            byteRead = 5;
            break;
        case Msg_Board:
            std::cout << "Board" << std::endl;
            byteRead = 74;
            break;
        default:
            byteRead = 128;
            std::cout << "======UNKNOWN COMMAND======" << std::endl;
            break;
    }
    byteRead = read(fd, buffer, byteRead);
    if(byteRead <= 0){
        std::cout << "========= UNKNOWN ERROR =========" << std::endl;
        return false;
    }
    buffer[byteRead] = 0;

    std::vector<char> data(buffer, buffer + byteRead);
    std::lock_guard<std::mutex> lock(queue_lock);
    message_queue.push(data);
    return true;
}

std::vector<char> getTopMessage(){
    if(message_queue.empty()){
        return {};
    }
    std::vector<char> msg = message_queue.front();
    message_queue.pop();
    return msg;
}


