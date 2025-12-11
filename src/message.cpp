#include <message.h>
#include <mutex>

std::mutex queue_lock;
std::queue<std::pair<sf::TcpSocket *, std::vector<char>>> message_queue;

void sendMessage(sf::TcpSocket *soc, Msg messageType, std::string message) {
  std::cout << "SENDING MESSGAE" << std::endl;
  if (messageType == SET_NAME || messageType == TEAM_WHITE ||
      messageType == TEAM_BLACK) {
    std::string size;
    size += message.size();
    message = size + message;
  }
  char buffer[message.size() + 1];
  buffer[0] = messageType;
  message.copy(buffer + 1, message.size());
  soc->send(buffer, message.size() + 1);
}

bool lockMessage() { return queue_lock.try_lock(); }
void unlockMessage() { queue_lock.unlock(); }
int receiveMessage(sf::TcpSocket *soc) {
  std::cout << "RECEIVE MESSAGE" << std::endl;
  char buffer[128];
  std::size_t byteRead;
  soc->receive(buffer, 1, byteRead);
  std::cout << "MES REC" << std::endl;
  if (byteRead <= 0) {
    std::cout << "Client Disconnected" << std::endl;
    std::lock_guard<std::mutex> lock(queue_lock);
    message_queue.push({soc, {DISCONNECT}});
    return -1;
  }
  std::cout << "REceive msg: " << (int)buffer[0] << std::endl;
  Msg type = (Msg)buffer[0];
  switch (buffer[0]) {
  case CONNECT:
    byteRead = 17;
    break;
  case DISCONNECT: {
    std::lock_guard<std::mutex> lock(queue_lock);
    message_queue.push({soc, {DISCONNECT}});
    return 0;
  }
  case MOVE:
    byteRead = 5;
    break;
  case BOARD_SEND:
    std::cout << "RECEIVED BOARD" << std::endl;
    byteRead = 74;
    break;
  case BOARD_REQ:
    byteRead = 1;
    break;
  case START:
    byteRead = 1;
    break;
  case END:
    byteRead = 1;
    break;
  case INIT_CD:
    byteRead = 3;
    break;
  case RENEW_CD:
    byteRead = 2;
    break;
  case TEAM_WHITE:
  case TEAM_BLACK:
    soc->receive(buffer, 1, byteRead);
    if (byteRead <= 0) {
      std::cout << "========= UNKNOWN ERROR =========" << std::endl;
      return 0;
    }
    byteRead = buffer[0];
    break;
  case SET_NAME:
    std::cout << "SET NAME" << std::endl;
    soc->receive(buffer, 1, byteRead);
    if (byteRead <= 0) {
      std::cout << "========= UNKNOWN ERROR =========" << std::endl;
      return 0;
    }
    byteRead = buffer[0];
    break;
  default:
    byteRead = 127;
    std::string s = buffer;
    std::cout << "+++" << s << "+++" << (int)s[0] << std::endl;
    std::cout << "======UNKNOWN COMMAND======" << std::endl;
    std::cout << "======Clearing Pending Messages======" << std::endl;
    while (byteRead == 127) {
      soc->receive(buffer, 127, byteRead);
      s = buffer;
      std::cout << "+++" << s << "+++" << byteRead << std::endl;
    }
    return 0;
  }
  soc->receive(buffer, byteRead, byteRead);
  if (byteRead <= 0) {
    std::cout << "========= UNKNOWN ERROR =========" << std::endl;
    return 0;
  }
  buffer[byteRead] = type;
  buffer[byteRead + 1] = 0;

  std::vector<char> data(buffer, buffer + byteRead + 1);
  std::lock_guard<std::mutex> lock(queue_lock);
  message_queue.push({soc, data});
  return 1;
}

std::pair<sf::TcpSocket *, std::vector<char>> getTopMessage() {
  std::lock_guard<std::mutex> lock(queue_lock);
  if (message_queue.empty()) {
    return {NULL, {}};
  }
  std::pair<sf::TcpSocket *, std::vector<char>> msg = message_queue.front();
  message_queue.pop();
  return msg;
}
