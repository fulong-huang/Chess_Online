#include <iostream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <atomic>
// #include <sys/select.h>
// #include <unistd.h>
#include <vector>
// #include <mutex>
// #include <fcntl.h>
#include <board.h>
// #include <thread>
#include <message.h>
#include <cooldown.h>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

void initServer(int port);
void resetCooldowns();

void acceptClient();
void clientHandler(sf::TcpSocket* client_socket);
void readCommands();
void processCommands();

void stopServer();
void closeServer();

void startGame();
void endGame();
bool gameIsRunning();

void updateCD();

void splitTeam();
void sendBoard(sf::TcpSocket* client_fd);
void endGame(bool whiteTurn);

