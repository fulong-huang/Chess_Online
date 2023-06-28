#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 

enum Msg{
    Msg_Connect,
    Msg_Board,
    Msg_Move,
    
};

void sendMessage(int fd, Msg messageType, std::string message);

bool receiveMessage(int fd);

std::vector<char> getTopMessage();



