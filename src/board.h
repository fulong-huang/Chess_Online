#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <stdlib.h>
#include "pieces.h"


class ChessBoard{
public: 
    ChessBoard();
    void resetBoard();
    
    bool move(
        std::pair<int, int> from, std::pair<int, int> to, char promote = QUEEN
    );

    bool boardInCheck();
    bool gameIsRunning();
    bool compare(ChessBoard board2);
    bool isSelectable(std::pair<int, int> piece);
    bool isWhiteTurn();

    void printBoard();
    std::vector<char> getBoard();
    std::vector<char> getGameBoard();

    ChessBoard& operator=(ChessBoard board);
    void setBoard(std::vector<char> target, bool turn = true);
    std::vector<int> getValidMovements(int row, int col);

private:
    std::vector<char> board;
    bool whiteTurn;
    bool gameRunning;
    int whiteKingPos, blackKingPos;
    std::map<int, std::vector<int>> validMovements;
    int whitePassant, blackPassant;
    int whiteKingSideCastle, whiteQueenSideCastle;
    int blackKingSideCastle, blackQueenSideCastle;

    void switchTurn();
    void findValidMovements();
    bool validateMove(int from, int to);

    bool checkPawnMovement(std::pair<int, int> from, std::pair<int, int> to);
    bool checkRookMovement(std::pair<int, int> from, std::pair<int, int> to);
    bool checkKnightMovement(std::pair<int, int> from, std::pair<int, int> to);
    bool checkBishopMovement(std::pair<int, int> from, std::pair<int, int> to);
    bool checkQueenMovement(std::pair<int, int> from, std::pair<int, int> to);
    bool checkKingMovement(std::pair<int, int> from, std::pair<int, int> to);

    bool checkPawnMovement(int from, std::pair<int, int> to);
    bool checkRookMovement(int from, std::pair<int, int> to);
    bool checkKnightMovement(int from, std::pair<int, int> to);
    bool checkBishopMovement(int from, std::pair<int, int> to);
    bool checkQueenMovement(int from, std::pair<int, int> to);
    bool checkKingMovement(int from, std::pair<int, int> to);

    bool checkPawnMovement(int from, int to);
    bool checkRookMovement(int from, int to);
    bool checkKnightMovement(int from, int to);
    bool checkBishopMovement(int from, int to);
    bool checkQueenMovement(int from, int to);
    bool checkKingMovement(int from, int to);

    bool pieceAtPosition(int piece, std::pair<int, int> pos);

};

