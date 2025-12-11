// #include <cstdlib>
#include "SFML/Network/TcpSocket.hpp"
#include <mutex>
#include <server.h>

ChessBoard board;

bool server_running;
bool game_running;

sf::SocketSelector selector;
sf::TcpListener listener;

std::vector<sf::TcpSocket *> client_list;
std::vector<sf::TcpSocket *> teamBlack, teamWhite;

std::map<sf::TcpSocket *, std::string> client_names;

std::mutex client_list_lock;
std::mutex client_readers_lock;
std::mutex command_process_lock;
std::mutex board_lock;

std::vector<sf::Thread *> client_readers;

void splitTeam() {
  teamBlack.clear();
  teamWhite.clear();
  std::lock_guard<std::mutex> lock(client_list_lock);
  std::vector<sf::TcpSocket *> tempClient = client_list;
  bool b;
  while (!tempClient.empty()) {
    int randomIdx = std::rand() % tempClient.size();
    if (b) {
      teamBlack.push_back(tempClient[randomIdx]);
    } else {
      teamWhite.push_back(tempClient[randomIdx]);
    }
    b = !b;
    tempClient.erase(tempClient.begin() + randomIdx);
  }
}

void sendBoard(sf::TcpSocket *soc) {
  std::lock_guard<std::mutex> lock(board_lock);
  sendMessage(soc, BOARD_SEND, board.boardToString());
}

void initServer(int port) {
  server_running = true;
  // bind listener to port
  if (listener.listen(port) != sf::Socket::Done) {
    std::cout << "Listener failed to bind to port" << std::endl;
    exit(EXIT_FAILURE);
  }
  selector.add(listener);
}

void acceptClient() {
  while (server_running) {
    if (selector.wait(sf::seconds(1))) {
      if (selector.isReady(listener)) {
        sf::TcpSocket *client = new sf::TcpSocket;
        if (listener.accept(*client) == sf::Socket::Done) {
          {
            std::lock_guard<std::mutex> lock(client_list_lock);
            client_list.push_back(client);
            sf::Thread *new_client_thread =
                new sf::Thread(&readClientCommands, client);
            {
              std::lock_guard<std::mutex> lock(client_readers_lock);
              client_readers.push_back(new_client_thread);
              new_client_thread->launch();
            }
          }
          client->setBlocking(true);
          selector.add(*client);
        } else {
          delete client;
        }
      } else {
        sf::sleep(sf::seconds(1));
      }
    }
  }
  // sf::TcpSocket client;
  // sf::Thread read_command_thread(readCommands);
  // listener.setBlocking(false);
  // read_command_thread.launch();
  // while(server_running){
  //	if(listener.accept(client) == sf::Socket::Done){
  //		client.setBlocking(false);
  //		sendMessage(&client, CONNECT, "Hello From Server");
  //		{
  //			std::lock_guard<std::mutex> lock(board_lock);
  //			sendMessage(&client, BOARD_SEND, board.boardToString());
  //			std::cout << "SEND BOARD" << std::endl;
  //		}
  //		{
  //			std::lock_guard<std::mutex> lock(client_lock);
  //			client_list.push_back(&client);
  //		}
  //     }
  // }
  // read_command_thread.wait();
}

void clientHandler(sf::TcpSocket *client_socket) {

  while (receiveMessage(client_socket) && server_running) {
  }
  return;
}

void readClientCommands(sf::TcpSocket *client) {
  bool readingCommand = true;
  while (server_running && readingCommand) {
    std::cout << "Read Commands" << std::endl;
    int status;
    {
      if (selector.isReady(*client)) {
        if (receiveMessage(client) >= 0) {
          unlockCommandLock();
        } else {
          // Client disconnected
          readingCommand = false;
          unlockCommandLock();
        }
      } else {
        std::cout << "SELECTOR NOT READY" << std::endl;
      }
    }
  }
  std::cout << "RETURNING FROM READ CLIENT COMMAND" << std::endl;

  std::lock_guard<std::mutex> lock(client_readers_lock);
  // TODO:
  // Erase current thread from client_readers

  //	while(server_running){
  //		int status;
  //		std::lock_guard<std::mutex> lock(client_lock);
  //		for(sf::TcpSocket* i : client_list){
  //			status = receiveMessage(i);
  //			while(status > 0){
  //				status = receiveMessage(i);
  //			}
  //			if(status == -1){
  //				std::cout << "MES RECEIVE ERROR" << std::endl;
  //			}
  //		}
  //	}
}

void processCommands() {
  while (server_running) {
    std::cout << "Process Commands _____" << std::endl;
    command_process_lock.lock();
    sf::TcpSocket *client_fd;
    std::vector<char> msg;
    std::tie(client_fd, msg) = getTopMessage();
    if (msg.size() == 0) {
      std::cout << "NO MESSAGE " << std::endl;
      continue;
    }
    unlockCommandLock();
    std::string msgString(msg.begin(), msg.end() - 1);
    std::cout << "Received message: " << msgString << ";" << std::endl;

    char promote = QUEEN;
    Msg command_type = (Msg)msg[msg.size() - 1];
    if (command_type == MOVE) {
      promote = msg[4];
    } else if (command_type == BOARD_REQ) {
      std::lock_guard<std::mutex> lock(board_lock);
      sendMessage(client_fd, BOARD_SEND, board.boardToString());
      std::cout << "SEND BOARD" << std::endl;
      continue;
    } else if (command_type == DISCONNECT) {
      std::cout << "+++++++CLIENT DISCONNECT+++++++" << std::endl;
      std::lock_guard<std::mutex> lock(client_list_lock);
      std::vector<sf::TcpSocket *>::iterator position =
          std::find(client_list.begin(), client_list.end(), client_fd);
      if (position != client_list.end()) {
        std::cout << "ERASED: " << *position << std::endl;
        client_list.erase(position);
      } else {
        std::cout << "NOT ERASED" << std::endl;
      }
      continue;
    } else if (command_type == CONNECT) {
      std::cout << "CLIENT CONNECTION MESSAGE" << std::endl;
      continue;
    } else if (command_type == SET_NAME) {
      client_names[client_fd] = msgString;
    } else {
      std::cout << "Invalid message length" << std::endl;
      continue;
    }
    if (!game_running)
      continue;
    bool move_result, whiteTurn = board.isWhiteTurn();
    {
      std::chrono::steady_clock::time_point curr_time =
          std::chrono::steady_clock::now();
      if (!updateCooldown(curr_time, client_fd)) {
        std::cout << "Client " << client_fd
                  << " attempted to move before cooldown finished" << std::endl;
        std::string cdMessage = getCDAsMessage();
        sendMessage(client_fd, INIT_CD, cdMessage);
        continue;
      }
      move_result = board.move({msg[0] - '0', msg[1] - '0'},
                               {msg[2] - '0', msg[3] - '0'}, msg[4]);
    }
    sendMessage(client_fd, RENEW_CD, getPercentage(client_fd));
    if (move_result) {
      board.printBoard();
      {
        // disallow new client to connect
        std::lock_guard<std::mutex> lock(client_list_lock);
        for (sf::TcpSocket *i : client_list) {
          sendMessage(i, MOVE, msgString);
        }
        if (!board.gameIsRunning()) {
          std::lock_guard<std::mutex> lock(board_lock);
          std::string s;
          s += !whiteTurn;
          for (sf::TcpSocket *i : client_list) {
            sendMessage(i, END, s);
          }
          board.resetBoard();
          resetCooldowns();
        }
      }
      std::string s;
    } else {
      {
        std::lock_guard<std::mutex> lock(board_lock);
        sendMessage(client_fd, BOARD_SEND, board.boardToString());
        std::cout << "SEND BOARD" << std::endl;
      }
    }
  }
}

void initCooldowns() {
  std::chrono::steady_clock::time_point currTime =
      std::chrono::steady_clock::now();
  std::lock_guard<std::mutex> lock(client_list_lock);
  for (sf::TcpSocket *i : client_list) {
    updateCooldown(currTime, i);
  }
}

void unlockCommandLock() { command_process_lock.unlock(); }

void stopServer() {
  unlockCommandLock();
  server_running = false;
}

void closeServer() {
  // Server will clean up itself
  // close(server_fd);

  // --- Close all client connection ---
  {
    std::lock_guard<std::mutex> lock(client_list_lock);
    for (sf::TcpSocket *client : client_list) {
      sendMessage(client, DISCONNECT, "");
    }
  }
  {
    std::cout << "CLOSING" << std::endl;
    std::lock_guard<std::mutex> lock(client_readers_lock);
    for (sf::Thread *reader : client_readers) {
      std::cout << "FOR LOOP" << std::endl;

      reader->terminate();
      std::cout << "END FOR LOOP" << std::endl;
    }
  }
}

void startGame() {
  command_process_lock.unlock();
  initCooldowns();
  splitTeam();
  game_running = true;
  board.startGame();
  std::string cdMessage = getCDAsMessage();
  std::lock_guard<std::mutex> lock(board_lock);
  std::string boardString = board.boardToString();
  {
    std::lock_guard<std::mutex> lock(client_list_lock);
    std::cout << "SENDING BOARD TO " << client_list.size() << " clients"
              << std::endl;
    for (sf::TcpSocket *i : client_list) {
      sendMessage(i, BOARD_SEND, boardString);
      sendMessage(i, START, "s");
      sendMessage(i, INIT_CD, cdMessage);
      for (sf::TcpSocket *k : teamWhite) {
        sendMessage(i, TEAM_WHITE, client_names[k]);
      }
      for (sf::TcpSocket *k : teamBlack) {
        sendMessage(i, TEAM_BLACK, client_names[k]);
      }
    }
  }
}

void endGame() {
  resetCooldowns();
  game_running = false;
}

bool gameIsRunning() { return game_running; }

void updateCD() {
  if (game_running) {
    std::string cdMessage = getCDAsMessage();
    std::lock_guard<std::mutex> lock(client_list_lock);
    for (sf::TcpSocket *i : client_list) {
      sendMessage(i, INIT_CD, cdMessage);
      sendMessage(i, RENEW_CD, getPercentage(i));
    }
  }
}
