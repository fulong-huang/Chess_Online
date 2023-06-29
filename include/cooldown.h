#include <chrono>
#include <vector>
#include <iostream>
#include <map>

bool updateCooldown(std::chrono::steady_clock::time_point curr_time, int fd);
void resetCooldowns();
void setCooldownTime(int duration);
void setMaxMoves(int moves);


