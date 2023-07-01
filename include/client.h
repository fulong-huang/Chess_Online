#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <message.h>
#include <fcntl.h>
#include <game.h>

void initClient(const char* server_ip, int port, const char* name);
void messageReceiver();
void messageSender(Msg messageType, std::string msg);
void processCommands();

void runGame();
std::string combinePositions(std::pair<int, int> from, std::pair<int, int> to);


