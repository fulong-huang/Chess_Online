#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <queue> 
#include <SFML/Network.hpp>

enum Msg{
    CONNECT,
	DISCONNECT,
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

void sendMessage(sf::TcpSocket *soc, Msg messageType, std::string message);

int receiveMessage(sf::TcpSocket *soc);

std::pair<sf::TcpSocket*, std::vector<char>> getTopMessage();



