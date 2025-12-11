#include <SFML/System/Thread.hpp>
#include <client.h>
#include <cstdlib>
#include <iostream>

int main(int argc, const char *argv[]) {
  if (argc != 4) {
    std::cout << "Invalid number of arguments, \n"
                 "\tExpected [Server IP address], [port], and [nick name]"
              << std::endl;
    return EXIT_FAILURE;
  }

  int port;
  try {
    port = std::stoi(argv[2]);
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  initClient(argv[1], port, argv[3]);
  messageSender(CONNECT, "Hello From Client");

  sf::Thread message_receiver(messageReceiver);
  sf::Thread command_processor(processCommands);

  message_receiver.launch();
  command_processor.launch();

  runGame();
  std::cout << "Game Ended" << std::endl;

  messageSender(DISCONNECT, "Disconnect from client");
  message_receiver.terminate();
  command_processor.wait();

  return 0;
}
