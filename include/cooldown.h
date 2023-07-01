#include <chrono>
#include <vector>
#include <iostream>
#include <map>

bool updateCooldown(std::chrono::steady_clock::time_point curr_time, int fd);
void resetCooldowns();
void setCooldownTime(int duration);
int getCooldownTime();
void setMaxMoves(int moves);
int getMaxMoves();
std::string getPercentage(int fd);

int getClientCooldown(int client_fd);

std::string getCDAsMessage();


