#include "game.h"

Game::Game(){
    this->window.create(
            sf::VideoMode(900, 600), 
            "Chess", 
            sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize
            );
    this->window.setFramerateLimit(60);

    this->moves_front = NULL;
    this->moves_back = NULL;
    this->moves_count = 0;
    this->initGame();
    this->view.setSize(1000, 1000);
    this->boardSize = {700, 950};
    this->gridSize = {this->boardSize.x/8.f, this->boardSize.y/8.f};
    this->resizeBoard();
    this->cd_curr = 0;
    this->cd_stored = 1;
    this->cd = 100;
    this->selected_promotion = 0;

    this->textureDisplaySize = 42;
    this->pieceScale = {
        (this->boardSize.x / 8 - 10) / this->textureDisplaySize,
        (this->boardSize.y / 8 - 10) / this->textureDisplaySize};
    this->initTextures();
    this->initSprite();

    this->initText();
}

void Game::setViewPort(){
    sf::Vector2u winSize = this->window.getSize();
    float windowRatio = (float)(winSize.x) / (winSize.y);
    float viewRatio = this->boardSize.y / this->boardSize.x;
    std::cout << "View Ratio: " << viewRatio << std::endl;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    // If horizontalSpacing is true, the black bars will appear on the left and right side.
    // Otherwise, the black bars will appear on the top and bottom.

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }
   
    this->view.setViewport( sf::FloatRect(posX, posY, sizeX, sizeY) );
    //this->view.setViewport(sf::FloatRect(0, 0, 1, 1));
}

void Game::resizeBoard(){
    this->setViewPort();
    this->window.setView(this->view);
}

void Game::initGame(){
    std::cout << "LKSJDLFKJ" << std::endl;
    this->board.resetBoard();
    this->currBoard = this->board.getGameBoard();
    this->moveFrom = {-1, -1};
    this->promotion = false;
    this->prevFrom = {-1, -1};
    this->validTargets = {};
    this->whiteTurn = this->board.isWhiteTurn();
    this->clearHistory();
}

void Game::clearHistory(){
    while(moves_front != NULL){
        PastMoves* curr = moves_front;
        moves_front = moves_front->next;
        delete curr;
    }
    moves_front = NULL;
    moves_back = NULL;
    moves_count = 0;
}

// Check for return value, if value incorrect, request for new board.
bool Game::move(std::pair<int, int> from,
        std::pair<int, int> to, 
        char piece){
    if(this->board.move(from, to, piece)){
        PastMoves* currMove = new struct PastMoves();
        currMove->next = moves_front;
        currMove->prev = NULL;
        currMove->moves = {from.first, from.second, to.first, to.second};
        if(moves_front != NULL){
            moves_front->prev = currMove;
        }
        moves_front = currMove;
        moves_count++;
        if(moves_back  == NULL){
            moves_back = currMove;
        }
        else if(moves_count >= 5){
            moves_back = moves_back->prev;
            delete moves_back->next;
            moves_back->next = NULL;
            moves_count--;
        }
    }
    else{
        std::cout << "Unable to perform Move" << std::endl;
    }
    this->currBoard = this->board.getGameBoard();

    return true;
}

void Game::startGame(){
    this->clearHistory();
    this->board.startGame();
    this->team_black.clear();
    this->team_white.clear();
    this->cd_curr = 0;
}

bool Game::handleMouseClick(){
    if(!this->board.gameIsRunning()){
        //this->initGame();
        return false;
    }
    sf::Vector2f mousePos = this->window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
    if(mousePos.x < 0 || mousePos.y < 0 || 
            mousePos.x >= 1000 || mousePos.y > this->boardSize.y){
        this->validTargets = {};
        this->moveFrom = {-1, -1};
        this->promotion = false;
        return false;
    }
    if(mousePos.x > this->boardSize.x ){
        if(mousePos.y <= 556 && mousePos.y >= 467){
            for(int i = 0; i < 4; i++){
                int startPos = 710 + i * 75;
                if(mousePos.x >= startPos &&
                        mousePos.x <= startPos + 65){
                    this->selected_promotion = i;
                    return false;
                }
            }
        }
        this->validTargets = {};
        this->moveFrom = {-1, -1};
        this->promotion = false;
        return false;
    }
    if(this->promotion){
        this->promotion = false;
        char targetPiece;
        if(mousePos.x <= 2 * this->gridSize.x){
            targetPiece = ROOK;
        }
        else if(mousePos.x <= 4 * this->gridSize.x){
            targetPiece = KNIGHT;
        }
        else if(mousePos.x <= 6 * this->gridSize.x){
            targetPiece = BISHOP;
        }
        else{
            targetPiece = QUEEN;
        }
        // TODO
        if(this->board.move(this->moveFrom, this->moveTo, targetPiece)){
            this->currBoard = this->board.getGameBoard();
            this->prevFrom = this->moveFrom;
            this->prevTo = this->moveTo;
            if(!this->board.gameIsRunning()){
                if(this->whiteTurn){
                    this->overlayText.setString("White\nWin!!!");
                }
                else{
                    this->overlayText.setString("Black\nWin!!!");
                }
            }
        }
        this->whiteTurn = this->board.isWhiteTurn();
        this->moveFrom = {-1, -1};
        this->validTargets = {};
        return false;
    }
    int row, col;
    row = mousePos.y / this->gridSize.y;
    col = mousePos.x / this->gridSize.x;
    if(this->board.isSelectable({row, col})){
        if(row == this->moveFrom.first && col == this->moveFrom.second){
            this->moveFrom = {-1, -1};
            this->validTargets = {};
            return false;
        }
        this->validTargets = this->board.getValidMovements(row, col);
        this->moveFrom = {row, col};
        return false;
    }
    if(this->moveFrom.first != -1){
        int currIdx = row * 8 + col;
        bool isPawn = this->currBoard[this->moveFrom.first * 8 + 
                                    this->moveFrom.second] % 10 == PAWN;
        bool reachedEnd = (row == 0 || row == 7);
        // TODO 
        isPawn = false;
        this->moveTo = {row, col};
        if(isPawn && reachedEnd){
            bool isValidTarget = false;
            for(int validTar : this->validTargets){
                if(validTar == currIdx){
                    isValidTarget = true;
                    break;
                }
            }
            if(!isValidTarget){
                this->moveFrom = {-1, -1};
                this->validTargets = {};
                return false;
            }
            this->promotion = true;
            return false;
        }
        this->savedFrom = this->moveFrom;
        this->savedTo = this->moveTo;
        bool success = (std::find(this->validTargets.begin(),
                                    this->validTargets.end(),
                                    currIdx) != this->validTargets.end());
        this->moveFrom.first = -1;
        return success;
                                    
//        bool success = this->board.move(this->moveFrom, {row, col});
//        if(success){
//            this->currBoard = this->board.getGameBoard();
//            this->prevFrom = this->moveFrom;
//            this->prevTo = {row, col};
//            if(!this->board.gameIsRunning()){
//                if(this->whiteTurn){
//                    this->gameOverText.setString("White\nWin!!!");
//                }
//                else{
//                    this->gameOverText.setString("Black\nWin!!!");
//                }
//            }
//        }
//        this->whiteTurn = this->board.isWhiteTurn();
//        this->validTargets.clear();
//        this->moveFrom = {-1, -1};
    }
    return false;
}

void Game::display(){
    this->window.clear();
    this->window.setView(this->view);

    this->window.draw(this->spriteLists[0]);
    this->displaySideBar();
    this->displayTeams();

    this->drawPieces();
    this->displayOverlay();
    this->window.display();
}

void Game::displayTeams(){
    this->team_text.setFont(this->font);
    this->team_text.setFillColor(sf::Color::Black);
    
    this->team_text.setString("BLACK");
    this->team_text.setCharacterSize(50);
    this->team_text.setPosition(
            850 - this->team_text.getLocalBounds().width/2,
            this->team_text.getLocalBounds().height/2
            );
    this->window.draw(this->team_text);

    this->team_text.setCharacterSize(30);
    for(int i = 0; i < team_black.size(); i++){
        this->team_text.setString(team_black[i]);
        this->team_text.setPosition(
                850 - this->team_text.getLocalBounds().width/2,
                (i * 35) + 80
                );
        if(team_black[i].compare(this->player_name) == 0){
            this->team_text.setFillColor(sf::Color::Green);
            this->window.draw(this->team_text);
            this->team_text.setFillColor(sf::Color::Black);
        }
        else{
            this->window.draw(this->team_text);
        }
    }
    this->team_text.setFillColor(sf::Color::White);

    this->team_text.setString("WHITE");
    this->team_text.setCharacterSize(50);
    this->team_text.setPosition(
            850 - this->team_text.getLocalBounds().width/2,
            1000 - this->team_text.getLocalBounds().height * 2.8
            );
    this->window.draw(this->team_text);

    this->team_text.setCharacterSize(30);
    for(int i = 0; i < team_white.size(); i++){
        this->team_text.setString(team_white[i]);
        this->team_text.setPosition(
                850 - this->team_text.getLocalBounds().width/2,
                870 - (i * 35)
                );
        if(team_white[i].compare(this->player_name) == 0){
            this->team_text.setFillColor(sf::Color::Green);
            this->window.draw(this->team_text);
            this->team_text.setFillColor(sf::Color::White);
        }
        else{
            this->window.draw(this->team_text);
        }
    }
}

void Game::addWhiteTeam(std::string name){
    this->team_white.push_back(name);
}
void Game::addBlackTeam(std::string name){
    this->team_black.push_back(name);
}

void Game::displaySideBar(){

    sf::RectangleShape r;
    r.setSize(sf::Vector2f(400, 446));
    r.setPosition(700, 0);
    r.setFillColor(sf::Color(135, 70, 5));
    this->window.draw(r);

    r.setPosition(700, 554);
    r.setFillColor(sf::Color(215, 115, 5));
    this->window.draw(r);
    r.setSize(sf::Vector2f(10, 1000));
    r.setPosition(700, 0);
    r.setFillColor(sf::Color::Black);
    this->window.draw(r);

    // Cooldown
    r.setPosition(0, 965);
    r.setFillColor(sf::Color::Black);
    r.setSize(sf::Vector2f(695, 25));

    r.setOutlineThickness(5);
    r.setOutlineColor(sf::Color(127, 127, 127));
    this->window.draw(r);
    r.setOutlineThickness(0);

    r.setPosition(0, 965);
    r.setFillColor(sf::Color::Green);
    float percentage = this->cd_curr / (float)(this->cd * this->cd_stored);
    r.setSize(sf::Vector2f(percentage * 700, 25));
    this->window.draw(r);
    
    r.setSize(sf::Vector2f(5, 25));
    r.setFillColor(sf::Color(127, 127, 127));
    float segment = 700.f / this->cd_stored;
    for(int i = 1; i < this->cd_stored; i++){
        r.setPosition(i * segment - 2, 965);
        this->window.draw(r);
    }
    r.setPosition(0, 965);
    this->window.draw(r);
    r.setPosition(695, 965);
    this->window.draw(r);

    // promotion pieces [13 - 16]
    sf::Color c;
    r.setSize(sf::Vector2f((65.f),(65.f * 950/700.f )));

    for(int i = 0; i < 4; i ++){
        if(this->selected_promotion == i){
            c = sf::Color::Green;
            c.a = 128;
        }
        else{
            c = sf::Color(128, 128, 128);
        }
        r.setFillColor(c);
        r.setPosition(710 + i * 75, 457.5);
        this->window.draw(r);
    }

    this->spriteLists[13].setPosition(715, 462.5);
    this->window.draw(this->spriteLists[13]);
    this->spriteLists[14].setPosition(790, 462.5);
    this->window.draw(this->spriteLists[14]);
    this->spriteLists[15].setPosition(865, 462.5);
    this->window.draw(this->spriteLists[15]);
    this->spriteLists[16].setPosition(940, 462.5);
    this->window.draw(this->spriteLists[16]);

}

void Game::drawPieces(){
    sf::RectangleShape lastMove;
    lastMove.setSize({this->gridSize.x + 2, this->gridSize.y + 2});
    int opacity = 128;
    PastMoves* curr = moves_front;
    while(curr != NULL){
        std::vector<int> v = curr->moves;
        lastMove.setFillColor(sf::Color(50, 200, 50, opacity));
        lastMove.setPosition(
                    v[1] * this->gridSize.x - 1,
                    v[0] * this->gridSize.y - 1
                );
        this->window.draw(lastMove);
        lastMove.setPosition(
                    v[3] * this->gridSize.x - 1,
                    v[2] * this->gridSize.y - 1
                );
        this->window.draw(lastMove);
        opacity -= 30;
        curr = curr->next;
    }

    float xPos, yPos;
    int i, spriteIdx;
    for(int idx = 0; idx < this->currBoard.size(); idx++){
        i = this->currBoard[idx];
        if(i == 0) continue;
        if(i > 10){
            spriteIdx = i % 10 + 6;
        }
        else{
            spriteIdx = i % 10;
        }
        
        xPos = this->gridSize.x * (idx % 8);
        yPos = this->gridSize.y * (int)(idx / 8);
        this->spriteLists[spriteIdx].setPosition(xPos + 5, yPos + 5);
        this->window.draw(this->spriteLists[spriteIdx]);
    }
}

void Game::updateCurrCD(int amount){
    if(!this->board.gameIsRunning()){
        return;
    }
    this->cd_curr += amount;
    if(this->cd_curr > this->cd * this->cd_stored){
        this->cd_curr = this->cd * this->cd_stored;
    }
}

void Game::initCD(std::vector<char> msg){
    if(msg[0] < 0){
        this->cd = 256 + msg[0];
    }
    else{
        this->cd = msg[0];
    }

    this->cd *= 255;

    if(msg[1] < 0){
        this->cd += (256 + msg[1]);
    }
    else{
        this->cd += msg[1];
    }

    if(msg[2] < 0){
        this->cd_stored = 256 + msg[2];
    }
    else{
        this->cd_stored = msg[2];
    }

    if(this->cd_curr > this->cd){
        this->cd_curr = this->cd;
    }
    std::cout << "CD: " << this->cd << ", " << this->cd_stored << std::endl;
}

void Game::updateServerCD(int percent){
    std::cout << "RECEIVED PERCENTAGE: " << percent << std::endl;
    this->cd_curr = (percent / 127.f) * (this->cd_stored * this->cd);
}

void Game::setCD(int newCD){
    this->cd = newCD;
}

void Game::setCDCurr(int newCD){
    this->cd_curr = newCD;
}

void Game::setCDStored(int amount){
    this->cd_stored = amount;
}

void Game::displayOverlay(){
    if(!this->board.gameIsRunning()){
        sf::RectangleShape rectOverlay;
        rectOverlay.setSize(sf::Vector2f(700, 950));
        rectOverlay.setFillColor(sf::Color(128, 128, 128, 192));
        this->window.draw(rectOverlay);
        this->window.draw(this->overlayText);
        this->window.draw(this->overlaySmallText);
        //this->window.draw(this->restartText);
        return;
    }
    
    if(this->moveFrom.first != -1){
        sf::CircleShape circle(1);
        circle.setScale({(this->boardSize.x / 16), (this->boardSize.y / 16)});
        circle.setFillColor(sf::Color(128, 128, 128, 192));
        circle.setPosition(
                this->moveFrom.second * this->gridSize.x,
                this->moveFrom.first * this->gridSize.y);
        this->window.draw(circle);

        circle.setScale({(this->boardSize.x / 32), (this->boardSize.y / 32)});
        for(int idx : this->validTargets){
            circle.setPosition( 
                    (idx % 8) * this->gridSize.x + this->gridSize.x / 4,
                    (int)(idx / 8) * this->gridSize.y +  this->gridSize.y / 4);            
            this->window.draw(circle);
        }
    }
    if(this->promotion){
        int mouseX = this->window.mapPixelToCoords(
                sf::Mouse::getPosition(this->window)).x;
        sf::Vector2f rectSize = {this->gridSize.x * 2, this->gridSize.y * 8};
        sf::RectangleShape darkRect(rectSize);
        sf::RectangleShape lightRect(rectSize);
        darkRect.setFillColor(sf::Color(128, 128, 128, 192));
        lightRect.setFillColor(sf::Color(128, 128, 128, 128));
        for(int i = 0; i < 4; i++){
            if(mouseX <= this->gridSize.x * 2 * (i + 1) &&
                    mouseX > this->gridSize.x * 2 * i){
                lightRect.setPosition(this->gridSize.x * 2 * i, 0);
                this->window.draw(lightRect);
            }
            else{
                darkRect.setPosition(this->gridSize.x * 2 * i, 0);
                this->window.draw(darkRect);
            }
        }
        sf::Sprite s;

        s.setScale(this->pieceScale.x * 2, this->pieceScale.y * 2);

        int pad = 6 * this->whiteTurn;
        s.setTexture(this->textureLists[2 + pad]);
        s.setPosition(0, this->gridSize.y * 3 + 10);
        this->window.draw(s);

        s.setTexture(this->textureLists[3 + pad]);
        s.setPosition(this->gridSize.x * 2 + 10, this->gridSize.y * 3 + 10);
        this->window.draw(s);
        
        s.setTexture(this->textureLists[4 + pad]);
        s.setPosition(this->gridSize.x * 4 + 10, this->gridSize.y * 3 + 10);
        this->window.draw(s);

        s.setTexture(this->textureLists[5 + pad]);
        s.setPosition(this->gridSize.x * 6 + 10, this->gridSize.y * 3 + 10);
        this->window.draw(s);
    }
}

void Game::initTextures(){
    std::string srcDir = std::__fs::filesystem::path(__FILE__).parent_path();
    srcDir += "/../resources/";

    this->backgroundTexture.loadFromFile(srcDir + "imgs/chessboard.png");
    this->backgroundTexture.setRepeated(true);

    this->pawnTexture.loadFromFile(srcDir + "imgs/pawn1.png");
    this->rookTexture.loadFromFile(srcDir + "imgs/rook1.png");
    this->knightTexture.loadFromFile(srcDir + "imgs/knight1.png");
    this->bishopTexture.loadFromFile(srcDir + "imgs/bishop1.png");
    this->queenTexture.loadFromFile(srcDir + "imgs/queen1.png");
    this->kingTexture.loadFromFile(srcDir + "imgs/king1.png");

    this->wpawnTexture.loadFromFile(srcDir + "imgs/pawn.png");
    this->wrookTexture.loadFromFile(srcDir + "imgs/rook.png");
    this->wknightTexture.loadFromFile(srcDir + "imgs/knight.png");
    this->wbishopTexture.loadFromFile(srcDir + "imgs/bishop.png");
    this->wqueenTexture.loadFromFile(srcDir + "imgs/queen.png");
    this->wkingTexture.loadFromFile(srcDir + "imgs/king.png");

    this->pknightTexture.loadFromFile(srcDir + "imgs/knight2.png");
    this->pbishopTexture.loadFromFile(srcDir + "imgs/bishop2.png");
    this->pqueenTexture.loadFromFile(srcDir + "imgs/queen2.png");
    this->prookTexture.loadFromFile(srcDir + "imgs/rook2.png");

    this->textureLists = {
        backgroundTexture, 

        pawnTexture, rookTexture, knightTexture, 
        bishopTexture, queenTexture, kingTexture,

        wpawnTexture, wrookTexture, wknightTexture, 
        wbishopTexture, wqueenTexture, wkingTexture,

        pqueenTexture, pbishopTexture, pknightTexture, prookTexture,
    };

    if(!this->font.loadFromFile(srcDir + "Silkscreen/slkscre.ttf")){
        std::cout << "Failed to load font" << std::endl;
    }
}

void Game::initSprite(){
    this->background.setTexture(this->backgroundTexture);
    float scale = this->backgroundTexture.getSize().x * 4;
    float scaley = this->backgroundTexture.getSize().y * 4;

    this->background.setTextureRect(sf::IntRect(0, 0, scale, scaley));
    this->background.setScale(this->boardSize.x / (scale - 1), this->boardSize.y / (scale - 4));
    
    this->spriteLists = {
        this->background,
        this->pawnSprite,
        this->rookSprite,
        this->knightSprite,
        this->bishopSprite,
        this->queenSprite,
        this->kingSprite,
        this->wpawnSprite,
        this->wrookSprite,
        this->wknightSprite,
        this->wbishopSprite,
        this->wqueenSprite,
        this->wkingSprite,
        this->pqueenSprite,
        this->prookSprite,
        this->pknightSprite,
        this->pbishopSprite,
    };
    for(int i = 1; i < 13; i++){
        this->spriteLists[i].setTexture(this->textureLists[i]);
        this->spriteLists[i].scale(this->pieceScale);
    }
    for(int i = 13; i < this->spriteLists.size(); i++){
        this->spriteLists[i].setTexture(this->textureLists[i]);
        this->spriteLists[i].scale(
                    (55.f/ this->textureDisplaySize),
                    (55.f * 950/700.f / this->textureDisplaySize)
                );
    }
    this->pieceScale = {
        (this->boardSize.x / 8 - 10) / this->textureDisplaySize,
        (this->boardSize.y / 8 - 10) / this->textureDisplaySize};
}

void Game::setWinner(bool white){
    if(white){
        this->overlayText.setFont(this->font);
        this->overlayText.setString("Winner\n.White.");
        this->overlayText.setCharacterSize(100); // 86
        float centerXPos = 
            (-this->boardSize.x + this->overlayText.getGlobalBounds().width) /2.f + 20;
        float centerYPos = 
            (-this->boardSize.y+this->overlayText.getGlobalBounds().height*2)/2.f;
        this->overlayText.setOrigin(centerXPos, centerYPos);
        this->overlayText.setStyle(sf::Text::Bold);
        this->overlayText.setFillColor(sf::Color::White);
        this->overlaySmallText.setFillColor(sf::Color::White);
    }
    else{
        this->overlayText.setFont(this->font);
        this->overlayText.setString("Winner\n.Black.");
        this->overlayText.setCharacterSize(100); // 86
        float centerXPos = 
            (-this->boardSize.x + this->overlayText.getGlobalBounds().width) /2.f + 20;
        float centerYPos = 
            (-this->boardSize.y+this->overlayText.getGlobalBounds().height*2)/2.f;
        this->overlayText.setOrigin(centerXPos, centerYPos);
        this->overlayText.setStyle(sf::Text::Bold);
        this->overlayText.setFillColor(sf::Color::Black);
        this->overlaySmallText.setFillColor(sf::Color::Black);
    }
}

void Game::initText(){
    this->overlayText.setFont(this->font);
    this->overlayText.setString("");
    this->overlayText.setCharacterSize(100); // 86
    float centerXPos = 
        (-this->boardSize.x + this->overlayText.getGlobalBounds().width) /2.f + 20;
    float centerYPos = 
        (-this->boardSize.y+this->overlayText.getGlobalBounds().height*2)/2.f;
    this->overlayText.setOrigin(centerXPos, centerYPos);
    this->overlayText.setFillColor(sf::Color::Black);
    this->overlayText.setStyle(sf::Text::Bold);

    this->overlaySmallText.setFont(this->font);
    this->overlaySmallText.setString("wait for server to start...");
    this->overlaySmallText.setCharacterSize(32); // 86
    centerXPos = 
        (-this->boardSize.x+this->overlaySmallText.getGlobalBounds().width)/2.f+20;
    centerYPos -= this->overlayText.getGlobalBounds().height + 100; 
    this->overlaySmallText.setOrigin(centerXPos, centerYPos);
    this->overlaySmallText.setFillColor(sf::Color::Black);
    this->overlaySmallText.setStyle(sf::Text::Bold);
//    this->restartText.setFont(this->font);
//    this->restartText.setString("");
//    this->restartText.setCharacterSize(32);
//    this->restartText.setPosition(this->gridSize.x * 1.5 + 16, 
//                                    this->gridSize.y * 5);
//    this->restartText.setFillColor(sf::Color::Black);
}

void Game::setTeam(std::vector<char> members, bool white){
    if(white){

    }
}

bool Game::stringToBoard(std::vector<char> buffer, int bufferSize){
    bool result = this->board.stringToBoard(buffer, bufferSize);
    this->whiteTurn = this->board.isWhiteTurn();
    this->currBoard = this->board.getGameBoard();
    return result;
}

