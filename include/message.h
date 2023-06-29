#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 

enum Msg{
    CONNECT,
    BOARD_SEND,
    BOARD_REQ,
    MOVE,
    
};

void sendMessage(int fd, Msg messageType, std::string message);

bool receiveMessage(int fd);

std::pair<int, std::vector<char>> getTopMessage();



