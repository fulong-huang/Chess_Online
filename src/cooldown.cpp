#include <cooldown.h>

// key:     client_fd
// value:   time from last command, exceed time after command
std::map<
    int, 
    std::pair<std::chrono::steady_clock::time_point, float>
    > client_cooldowns;
int cooldown = 2000;
int maxMoves = 3;

bool updateCooldown(std::chrono::steady_clock::time_point curr_time, int fd){
    bool result = false;
    if(client_cooldowns.find(fd) == client_cooldowns.end()){
        client_cooldowns[fd] = {curr_time, 0};
    }
    else{
        std::chrono::steady_clock::time_point past_time;
        float saved_time;
        std::tie(past_time, saved_time) = client_cooldowns[fd];
        saved_time += 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    curr_time - past_time
                    ).count();
        int maxSavedTime = cooldown * maxMoves;
        std::cout << "Current Saved Time: " << saved_time << std::endl;
        if(saved_time > maxSavedTime){
            saved_time = maxSavedTime;
        }
        if(saved_time >= cooldown){
            saved_time -= cooldown;
            result = true;
        }
        client_cooldowns[fd] = {curr_time, saved_time};
    }
    return result;
}

void resetCooldowns(){
    client_cooldowns.clear();
}



