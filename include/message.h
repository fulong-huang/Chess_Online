#include <SFML/Network.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <sys/socket.h>
#include <unistd.h>

enum Msg {
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

bool lockMessage();
void unlockMessage();

std::pair<sf::TcpSocket *, std::vector<char>> getTopMessage();
