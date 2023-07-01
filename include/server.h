#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <atomic>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <fcntl.h>
#include <board.h>
#include <thread>
#include <message.h>
#include <cooldown.h>

void initServer(int port);
void resetCooldowns();

void acceptClient();
void clientHandler(int client_socket);
void processCommands();

void stopServer();
void closeServer();

void startGame();
void endGame();
bool gameIsRunning();

void updateCD();

void splitTeam();


