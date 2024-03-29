#include <cooldown.h>

std::map<
    sf::TcpSocket*, 
    std::pair<std::chrono::steady_clock::time_point, float>
    > client_cooldowns;
int cooldown = 2000;
int maxMoves = 3;

std::string getPercentage(sf::TcpSocket* soc){
    if(client_cooldowns.find(soc) == client_cooldowns.end()){
        return "00";
    }
    float saved_time;
    std::chrono::steady_clock::time_point past_time;
    std::chrono::steady_clock::time_point curr_time = 
        std::chrono::steady_clock::now();

    std::tie(past_time, saved_time) = client_cooldowns[soc];
    saved_time += 
        std::chrono::duration_cast<std::chrono::milliseconds>(
                curr_time - past_time
                ).count();
    int maxSavedTime = cooldown * maxMoves;
    if(saved_time > maxSavedTime){
        saved_time = maxSavedTime;
    }
    client_cooldowns[soc] = {curr_time, saved_time};

    std::string result;
    result += (int)((saved_time / (cooldown * maxMoves)) * 127);
    result += 'r';
    return result;
}

bool updateCooldown(std::chrono::steady_clock::time_point curr_time, sf::TcpSocket* soc){
    bool result = false;
    if(client_cooldowns.find(soc) == client_cooldowns.end()){
        client_cooldowns[soc] = {curr_time, 0};
    }
    else{
        std::chrono::steady_clock::time_point past_time;
        float saved_time;
        std::tie(past_time, saved_time) = client_cooldowns[soc];
        saved_time += 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    curr_time - past_time
                    ).count();
        int maxSavedTime = cooldown * maxMoves;
        if(saved_time > maxSavedTime){
            saved_time = maxSavedTime;
        }
        if(saved_time >= cooldown){
            saved_time -= cooldown;
            result = true;
        }
        client_cooldowns[soc] = {curr_time, saved_time};
    }
    return result;
}

std::string getCDAsMessage(){
    std::string result = "   ";
    result[0] = cooldown / 255;
    result[1] = cooldown % 255;
    result[2] = maxMoves;
    return result;
}

int getClientCooldown(sf::TcpSocket* soc){
    if(client_cooldowns.find(soc) == client_cooldowns.end()){
        return 0;
    }
    return client_cooldowns[soc].second;
}

void resetCooldowns(){
    client_cooldowns.clear();
}

int getCooldownTime(){
    return cooldown;
}

int getMaxMoves(){
    return maxMoves;
}

void setCooldownTime(int duration){
    if(duration >= (255 * 255)){
        std::cout << 
            "Cooldown too short, please enter value greater or equal to 10." <<
            " (time unit is [ms])" << std::endl;
        return;
    }
    if(duration <= 10){
        std::cout << 
            "Cooldown too short, please enter value greater or equal to 10." <<
            " (time unit is [ms])" << std::endl;
        return;
    }
    cooldown = duration;
    std::cout << "Cooldown now set to: " <<
        (float)duration / 1000 << " Seconds" << std::endl;
}

void setMaxMoves(int moves){
    if(moves >= 255){
        std::cout << 
            "Invalid moves, please enter a value less than 255" <<
            std::endl;
        return;
    }
    if(moves <= 0){
        std::cout << 
            "Invalid moves, please enter a non-zero positive number" << 
            std::endl;
        return;
    }
    maxMoves = moves;
    std::cout << "Maximum moves is now: " << maxMoves << std::endl;
}


