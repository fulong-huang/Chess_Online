#include <iostream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <atomic>
// #include <sys/select.h>
// #include <unistd.h>
#include <mutex>
#include <vector>
// #include <fcntl.h>
#include <board.h>
// #include <thread>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <cooldown.h>
#include <message.h>

void initServer(int port);
void resetCooldowns();

void acceptClient();
void clientHandler(sf::TcpSocket *client_socket);
void readClientCommands(sf::TcpSocket *client);
void processCommands();

void stopServer();
void closeServer();

void startGame();
void endGame();
bool gameIsRunning();

void unlockCommandLock();

void updateCD();

void splitTeam();
void sendBoard(sf::TcpSocket *client_fd);
void endGame(bool whiteTurn);
