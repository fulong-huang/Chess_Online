#include <iostream>
#include <SFML/Graphics.hpp>
#include "board.h"
#include <filesystem>


class Game{

public:
    sf::RenderWindow window;
    sf::Event event;

    // TODO
    std::pair<int, int> savedFrom;
    std::pair<int, int> savedTo;

    std::string player_name;

    Game();
    void display();
    void resizeBoard();
    bool move(std::pair<int, int> from, std::pair<int, int> to);
    bool handleMouseClick();
    std::pair<int, int> moveFrom;
    std::pair<int, int> moveTo;
    void startGame();
    void setWinner(bool white);
    
    bool stringToBoard(std::vector<char> buffer, int bufferSize);

    void setCD(int newCD);
    void setCDCurr(int newCD);
    void setCDStored(int amount);
    void initCD(std::vector<char> msg);
    void updateCurrCD(int amount);
    void updateServerCD(int percent);

    void setTeam(std::vector<char> members, bool white);
    void addWhiteTeam(std::string name);
    void addBlackTeam(std::string name);

private:
    struct PastMoves{
        std::vector<int> moves;
        PastMoves* prev;
        PastMoves* next;
    };
    int moves_count;
    PastMoves* moves_front;
    PastMoves* moves_back;
    sf::View view;
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
    sf::Text overlayText;
    sf::Text overlaySmallText;
    sf::Text team_text;

    int textureDisplaySize;
    bool whiteTurn;
    std::pair<int, int> prevFrom;
    std::pair<int, int> prevTo;
    std::vector<int> validTargets;

    ChessBoard board;
    std::vector<char> currBoard;

    bool promotion;
    int cd; // cooldown time
    int cd_curr; // current progress
    int cd_stored;  // Total holding amount
    
    std::vector<std::string> team_black, team_white;
    void displayTeams();

    std::pair<float, float> padding;
    sf::Vector2f boardSize;
    sf::Vector2f gridSize;
    sf::Vector2f pieceScale;

    void setViewPort();
    void initGame();
    void initTextures();
    void initText();
    void initSprite();
    void drawPieces();
    void displayOverlay();
    void displaySideBar();
    void clearHistory();
};

