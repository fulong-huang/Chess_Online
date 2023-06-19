#include <iostream>
#include <SFML/Graphics.hpp>
#include "board.h"
#include <filesystem>


class Game{

public:
    Game();

    void update();
    void display();

    bool isRunning();
    void resizeBoard();



private:
    sf::View view;
    sf::RenderWindow window;
    sf::Event event;
    std::vector<sf::Texture> textureLists;
    std::vector<sf::Sprite> spriteLists;

    sf::Texture backgroundTexture;
    sf::Texture pawnTexture;
    sf::Texture rookTexture;
    sf::Texture knightTexture;
    sf::Texture bishopTexture;
    sf::Texture queenTexture;
    sf::Texture kingTexture;
    sf::Texture wpawnTexture;
    sf::Texture wrookTexture;
    sf::Texture wknightTexture;
    sf::Texture wbishopTexture;
    sf::Texture wqueenTexture;
    sf::Texture wkingTexture;

    sf::Sprite background;
    sf::Sprite pawnSprite;
    sf::Sprite rookSprite;
    sf::Sprite knightSprite;
    sf::Sprite bishopSprite;
    sf::Sprite queenSprite;
    sf::Sprite kingSprite;
    sf::Sprite wpawnSprite;
    sf::Sprite wrookSprite;
    sf::Sprite wknightSprite;
    sf::Sprite wbishopSprite;
    sf::Sprite wqueenSprite;
    sf::Sprite wkingSprite;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text restartText;

    bool running;
    int textureDisplaySize;
    bool whiteTurn;
    std::pair<int, int> moveFrom;
    std::pair<int, int> moveTo;
    std::pair<int, int> prevFrom;
    std::pair<int, int> prevTo;
    std::vector<int> validTargets;

    ChessBoard board;
    std::vector<char> currBoard;

    bool promotion;
    
    std::pair<float, float> padding;
    sf::Vector2f boardSize;
    sf::Vector2f gridSize;
    sf::Vector2f pieceScale;

    void setViewPort();
    void initGame();
    void handleMouseClick();
    void initTextures();
    void initText();
    void initSprite();
    void drawPieces();
    void displayOverlay();
    void displaySideBar();
    
};

