#include <chrono>
#include <vector>
#include <iostream>
#include <map>
#include <SFML/Network.hpp>

bool updateCooldown(std::chrono::steady_clock::time_point curr_time, sf::TcpSocket* soc);
void resetCooldowns();
void setCooldownTime(int duration);
int getCooldownTime();
void setMaxMoves(int moves);
int getMaxMoves();
std::string getPercentage(sf::TcpSocket* soc);

int getClientCooldown(sf::TcpSocket* soc);

std::string getCDAsMessage();


