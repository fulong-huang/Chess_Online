#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 
#include <SFML/Network.hpp>

enum Msg{
    CONNECT,
    BOARD_SEND,
    BOARD_REQ,
    MOVE,
    START,
    END,
    INIT_CD,
    RENEW_CD,
    TEAM_WHITE,
    TEAM_BLACK,
    SET_NAME,
};

void sendMessage(int fd, Msg messageType, std::string message);
void sendMessage(sf::TcpSocket *soc, Msg messageType, std::string message);

bool receiveMessage(int fd);
bool receiveMessage(sf::TcpSocket *soc);

std::pair<int, std::vector<char>> getTopMessage();



