#include "game.h"

Game::Game(){
    this->window.create(
            sf::VideoMode(800, 600), 
            "Chess", 
            sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize
            );
    this->window.setFramerateLimit(15);


    this->initGame();
    this->view.setSize(1000, 1000);
    this->resizeBoard();
    this->gridSize = {800.0/8, 125};
    this->boardSize = {800, 1000};

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
    float viewRatio = 1000.f / 800;
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
    this->running = true;
    this->board.resetBoard();
    this->currBoard = this->board.getGameBoard();
    this->moveFrom = {-1, -1};
    this->promotion = false;
    this->prevFrom = {-1, -1};
    this->validTargets = {};
    this->whiteTurn = this->board.isWhiteTurn();
}

void Game::handleMouseClick(){
    if(!this->board.gameIsRunning()){
        this->initGame();
        return;
    }
    sf::Vector2f mousePos = this->window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
    if(mousePos.x < 0 || mousePos.y < 0 || 
            mousePos.x > this->boardSize.x || mousePos.y > this->boardSize.y){
        this->validTargets = {};
        this->moveFrom = {-1, -1};
        this->promotion = false;
        return;
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
        if(this->board.move(this->moveFrom, this->moveTo, targetPiece)){
            this->currBoard = this->board.getGameBoard();
            this->prevFrom = this->moveFrom;
            this->prevTo = this->moveTo;
            if(!this->board.gameIsRunning()){
                if(this->whiteTurn){
                    this->gameOverText.setString("White\nWin!!!");
                }
                else{
                    this->gameOverText.setString("Black\nWin!!!");
                }
            }
        }
        this->whiteTurn = this->board.isWhiteTurn();
        this->moveFrom = {-1, -1};
        this->validTargets = {};
        return;
    }
    int row, col;
    row = mousePos.y / this->gridSize.y;
    col = mousePos.x / this->gridSize.x;
    if(this->board.isSelectable({row, col})){
        if(row == this->moveFrom.first && col == this->moveFrom.second){
            this->moveFrom = {-1, -1};
            this->validTargets = {};
            return;
        }
        this->validTargets = this->board.getValidMovements(row, col);
        this->moveFrom = {row, col};
        return;
    }
    if(this->moveFrom.first != -1){
        int currIdx = row * 8 + col;
        bool isPawn = this->currBoard[this->moveFrom.first * 8 + 
                                    this->moveFrom.second] % 10 == PAWN;
        bool reachedEnd = (row == 0 || row == 7);
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
                return;
            }
            this->promotion = true;
            this->moveTo = {row, col};
            return;
        }
        bool success = this->board.move(this->moveFrom, {row, col});
        if(success){
            this->currBoard = this->board.getGameBoard();
            this->prevFrom = this->moveFrom;
            this->prevTo = {row, col};
            if(!this->board.gameIsRunning()){
                if(this->whiteTurn){
                    this->gameOverText.setString("White\nWin!!!");
                }
                else{
                    this->gameOverText.setString("Black\nWin!!!");
                }
            }
        }
        this->whiteTurn = this->board.isWhiteTurn();
        this->validTargets.clear();
        this->moveFrom = {-1, -1};
    }
    else{
//        this->validTargets = this->board.getValidMovements(row, col);
//        this->moveFrom = {row, col};
    }
}

void Game::update(){
    while(this->window.pollEvent(this->event)){
        switch(this->event.type){
            case sf::Event::Closed:
                {
                    this->running = false;
                    break;
                }
            case sf::Event::Resized:
                {
                    this->resizeBoard();
                    break;
                }
            case sf::Event::MouseButtonPressed:
                {
                    this->handleMouseClick();
                }
            default:
                break;
        }
    }
}

void Game::display(){
    this->window.clear();
    this->window.setView(this->view);

    this->window.draw(this->spriteLists[0]);
    this->displaySideBar();

    this->drawPieces();
    this->displayOverlay();
    this->window.display();
}

void Game::displaySideBar(){

    sf::RectangleShape r;
    r.setSize(sf::Vector2f(300, 500));
    r.setPosition(800, 0);
    r.setFillColor(sf::Color(215, 115, 5));
    this->window.draw(r);

    r.setPosition(800, 500);
    r.setFillColor(sf::Color(135, 70, 5));
    this->window.draw(r);
}

void Game::drawPieces(){
    if(prevFrom.first != -1){
        sf::RectangleShape lastMove;
        lastMove.setSize({this->gridSize.x + 2, this->gridSize.y + 2});
        lastMove.setFillColor(sf::Color(50, 200, 50, 128));
        lastMove.setPosition(
                prevFrom.second * this->gridSize.x - 1,
                prevFrom.first * this->gridSize.y - 1);
        this->window.draw(lastMove);
        lastMove.setPosition(
                prevTo.second * this->gridSize.x - 1, 
                prevTo.first * this->gridSize.y - 1);
        this->window.draw(lastMove);
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

void Game::displayOverlay(){
    if(!this->board.gameIsRunning()){
        sf::RectangleShape rectOverlay;
        rectOverlay.setSize(this->boardSize);
        rectOverlay.setFillColor(sf::Color(128, 128, 128, 192));
        this->window.draw(rectOverlay);
        this->window.draw(this->gameOverText);
        this->window.draw(this->restartText);
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

bool Game::isRunning(){
    return this->running;
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


    this->textureLists = {
        backgroundTexture, 

        pawnTexture, rookTexture, knightTexture, 
        bishopTexture, queenTexture, kingTexture,

        wpawnTexture, wrookTexture, wknightTexture, 
        wbishopTexture, wqueenTexture, wkingTexture,
    };

    if(!this->font.loadFromFile(srcDir + "Silkscreen/slkscre.ttf")){
        std::cout << "Failed to load font" << std::endl;
    }
}

void Game::initSprite(){
    this->background.setTexture(this->backgroundTexture);
    float scale = this->backgroundTexture.getSize().x * 4;

    this->background.setTextureRect(sf::IntRect(0, 0, scale, scale));
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
    };
    for(int i = 1; i < spriteLists.size(); i++){
        this->spriteLists[i].setTexture(this->textureLists[i]);
        this->spriteLists[i].scale(this->pieceScale);
    }
}

void Game::initText(){
    this->gameOverText.setFont(this->font);
    this->gameOverText.setString("ABCDE\nWIN!!!");
    this->gameOverText.setCharacterSize(150); // 86
    this->gameOverText.setPosition( this->gridSize.x * 1 + 18,
                                    this->gridSize.y * 2 + 10);
    this->gameOverText.setFillColor(sf::Color::Black);
    this->gameOverText.setStyle(sf::Text::Bold);

    this->restartText.setFont(this->font);
    this->restartText.setString("Left Click to Restart");
    this->restartText.setCharacterSize(32);
    this->restartText.setPosition(this->gridSize.x * 1.5 + 16, 
                                    this->gridSize.y * 5);
    this->restartText.setFillColor(sf::Color::Black);
}


