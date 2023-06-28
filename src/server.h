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

void initServer(int port);

void acceptClient();
void clientHandler(int client_socket);
void processCommands();




