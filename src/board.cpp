#include "board.h"
#include <string>

ChessBoard::ChessBoard(){
    this->whiteTurn = true;
    this->board = {
    //   0      1       2       3       4       5       6       7
        ROOK,   KNIGHT, BISHOP, QUEEN,  KING,   BISHOP, KNIGHT, ROOK,   // 0
        PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   // 1
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 2
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 3
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 4
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 5
        PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   // 6
        ROOK,   KNIGHT, BISHOP, QUEEN,  KING,   BISHOP, KNIGHT, ROOK,   // 7
    };
    for(int i = 48; i < this->board.size(); i++){
        this->board[i] += 10;
    }
    this->whiteKingPos = 60;
    this->blackKingPos = 4;
    this->findValidMovements();
    this->gameRunning = false;
    this->whitePassant = -64;
    this->blackPassant = -64;
    this->blackQueenSideCastle = true;
    this->whiteQueenSideCastle = true;
    this->blackKingSideCastle = true;
    this->whiteKingSideCastle = true;
}

bool ChessBoard::stringToBoard(std::vector<char> buffer, int bufferSize){
//    if(bufferSize != 74){
//        std::cout << "Invalid String Size" << std::endl;
//        return false;
//    }
    for(int i = 0; i < 64; i++){
        this->board[i] = buffer[i];
    }
    this->whiteTurn = buffer[64];
    this->gameRunning = buffer[65];
    if(buffer[66] != '-'){
        int val = buffer[66];
        val *= 10;
        val += buffer[67];
        this->whitePassant = val;
    }
    else{
        this->whitePassant = -64;
    }
    if(buffer[68] != '-'){
        int val = buffer[68];
        val *= 10;
        val += buffer[69];
        this->blackPassant = val;
    }
    else{
        this->blackPassant = -64;
    }
    this->blackQueenSideCastle = buffer[70];
    this->blackKingSideCastle = buffer[71];
    this->whiteQueenSideCastle = buffer[72];
    this->whiteKingSideCastle = buffer[73];
    this->findValidMovements();

    for(int i = 0; i < this->board.size(); i++){
        if(this->board[i] == KING){
            this->blackKingPos = i;
        }
        else if(this->board[i] == KING + 10){
            this->whiteKingPos = i;
        }
    }

    return true;
}

std::string ChessBoard::boardToString(){
    std::string result;
    for(int i = 0; i < this->board.size(); i++){
        result += this->board[i];
    }
    result += this->whiteTurn;
    result += gameRunning;
    if(this->whitePassant < 0){
        result += '-';
        result += '-';
    }
    else{
        int a = this->whitePassant / 10;
        int b = this->whitePassant % 10;
        result += a;
        result += b;
    }
    if(this->blackPassant < 0){
        result += '-';
        result += '-';
    }
    else{
        int a = this->blackPassant / 10;
        int b = this->blackPassant % 10;
        result += a;
        result += b;
    }
    result += this->blackQueenSideCastle;
    result += this->blackKingSideCastle;
    result += this->whiteQueenSideCastle;
    result += this->whiteKingSideCastle;

    return result;
}

void ChessBoard::resetBoard(){
    this->whiteTurn = true;
    this->board = {
    //   0      1       2       3       4       5       6       7
        ROOK,   KNIGHT, BISHOP, QUEEN,  KING,   BISHOP, KNIGHT, ROOK,   // 0
        PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   // 1
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 2
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 3
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 4
        EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  // 5
        PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   PAWN,   // 6
        ROOK,   KNIGHT, BISHOP, QUEEN,  KING,   BISHOP, KNIGHT, ROOK,   // 7
    };
    for(int i = 48; i < this->board.size(); i++){
        this->board[i] += 10;
    }
    this->whiteKingPos = 60;
    this->blackKingPos = 4;
    this->gameRunning = false;
    this->findValidMovements();
    this->whitePassant = -64;
    this->blackPassant = -64;
    this->blackQueenSideCastle = true;
    this->whiteQueenSideCastle = true;
    this->blackKingSideCastle = true;
    this->whiteKingSideCastle = true;
}

void ChessBoard::startGame(){
    this->gameRunning = true;
}

std::vector<int> ChessBoard::getValidMovements(int row, int col){
    int from = row * 8 + col;
    if(this->validMovements.find(from) == this->validMovements.end()){
        return {};
    }
    return this->validMovements[from];
}

bool ChessBoard::isWhiteTurn(){
    return this->whiteTurn;
}

void ChessBoard::findValidMovements(){
    this->validMovements.clear();
    for(int i = 0; i < this->board.size(); i++){
        if(this->board[i] == 0){
            continue;
        }
        if(this->board[i] > 10 != this->whiteTurn){
            continue;
        }
        int row = i / 8;
        int col = i % 8;
        this->validMovements[i] = {};
        switch(this->board[i] % 10){
            case PAWN:
                {
                    if(checkPawnMovement(i, {row + 1, col + 1}) &&
                            this->validateMove(i, i + 9)){
                        this->validMovements[i].push_back(i + 9);
                    }
                    if(checkPawnMovement(i, {row + 1, col - 1}) && 
                            this->validateMove(i, i + 7)){
                        this->validMovements[i].push_back(i + 7);
                    }
                    if(checkPawnMovement(i, {row - 1, col + 1}) && 
                            this->validateMove(i, i - 7)){
                        this->validMovements[i].push_back(i - 7);
                    }
                    if(checkPawnMovement(i, {row - 1, col - 1}) && 
                            this->validateMove(i, i - 9)){
                        this->validMovements[i].push_back(i - 9);
                    }
                    if(checkPawnMovement(i, {row + 1, col}) && 
                            this->validateMove(i, i + 8)){
                        this->validMovements[i].push_back(i + 8);
                    }
                    if(checkPawnMovement(i, {row - 1, col}) && 
                            this->validateMove(i, i - 8)){
                        this->validMovements[i].push_back(i - 8);
                    }
                    if(checkPawnMovement(i, {row + 2, col}) && 
                            this->validateMove(i, i + 16)){
                        this->validMovements[i].push_back(i + 16);
                    }
                    if(checkPawnMovement(i, {row - 2, col}) && 
                            this->validateMove(i, i - 16)){
                        this->validMovements[i].push_back(i - 16);
                    }
                    break;
                }
            case KNIGHT:
                {
                    if(checkKnightMovement(i, {row + 1, col + 2}) &&
                            this->validateMove(i, i + 10)){
                        this->validMovements[i].push_back(i + 10);
                    }
                    if(checkKnightMovement(i, {row + 1, col - 2}) &&
                            this->validateMove(i, i + 6)){
                        this->validMovements[i].push_back(i + 6);
                    }
                    if(checkKnightMovement(i, {row - 1, col + 2}) && 
                            this->validateMove(i, i - 6)){
                        this->validMovements[i].push_back(i - 6);
                    }
                    if(checkKnightMovement(i, {row - 1, col - 2}) && 
                            this->validateMove(i, i - 10)){
                        this->validMovements[i].push_back(i - 10);
                    }

                    if(checkKnightMovement(i, {row + 2, col + 1}) &&
                            this->validateMove(i, i + 17)){
                        this->validMovements[i].push_back(i + 17);
                    }
                    if(checkKnightMovement(i, {row + 2, col - 1}) && 
                            this->validateMove(i, i + 15)){
                        this->validMovements[i].push_back(i + 15);
                    }
                    if(checkKnightMovement(i, {row - 2, col + 1}) &&
                            this->validateMove(i, i - 15)){
                        this->validMovements[i].push_back(i - 15);
                    }
                    if(checkKnightMovement(i, {row - 2, col - 1}) && 
                            this->validateMove(i, i - 17)){
                        this->validMovements[i].push_back(i - 17);
                    }
                    break;
                }
            case QUEEN:
            case ROOK:
                {
                    for(int x = i + 1; x % 8 != 0; x++){
                        if(checkRookMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i - 1; x % 8 != 7 && x >= 0; x--){
                        if(checkRookMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i + 8; x < this->board.size(); x += 8){
                        if(checkRookMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i - 8; x >= 0; x -= 8){
                        if(checkRookMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    if(this->board[i] % 10 == ROOK){
                        break;
                    }
                }
            case BISHOP:
                {
                    for(int x = i + 9; x % 8 != 0 && x < 64; x += 9){
                        if(checkBishopMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i + 7; (x - 7) % 8 != 0 && x < 64; x += 7){
                        if(checkBishopMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i - 9; (x + 9) % 8 != 0 && x >= 0; x -= 9){
                        if(checkBishopMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    for(int x = i - 7; x % 8 != 0 && x >= 0; x -= 7){
                        if(checkBishopMovement(i, x)){
                            if(this->validateMove(i, x)){
                                this->validMovements[i].push_back(x);
                            }
                        }
                        else{
                            break;
                        }
                    }
                    break;
                }
            case KING:
                {
                    for(int r = -1; r <= 1; r++){
                        for(int c = -1; c <= 1; c++){
                            if(checkKingMovement(i, {row + r, col + c})
                                    && this->validateMove(i, i+r*8+c)){
                                this->validMovements[i].push_back(i+r*8+c);
                            }
                        }
                    }
                    if(checkKingMovement(i, i + 2)){
                        this->validMovements[i].push_back(i + 2);
                    }
                    if(checkKingMovement(i, i - 2)){
                        this->validMovements[i].push_back(i - 2);
                    }
                    break;
                }
        }
        if(this->validMovements[i].size() == 0){
            this->validMovements.erase(i);
        }
    }
    
    if(this->validMovements.size() == 0){
        this->gameRunning = false;
    }
}

bool ChessBoard::validateMove(int from, int to){
    bool validMove = true;

    char curr = this->board[from];
    char target = this->board[to];
    int wKingPos = this->whiteKingPos;
    int bKingPos = this->blackKingPos;
    if(this->board[from] == KING){
        this->blackKingPos = to;
    }
    else if(this->board[from] == KING + 10){
        this->whiteKingPos = to;
    }
    this->board[to] = curr;
    this->board[from] = 0;
    if(boardInCheck()){
        validMove = false;
    }
    this->board[to] = target;
    this->board[from] = curr;
    this->blackKingPos = bKingPos;
    this->whiteKingPos = wKingPos;
    return validMove;
}

bool ChessBoard::move(
        std::pair<int, int> from, std::pair<int, int> to, char promote
    ){
    if(!this->gameRunning){
        std::cout << "Game Not Started!!!" << std::endl;
    }
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;


    if(curr < 0 || curr >= this->board.size()){
        return false;
    }
    if(target < 0 || target >= this->board.size()){ 
        return false;
    }

    if(this->validMovements.find(curr) == this->validMovements.end()){
        return false;
    }
    bool moveNotValid = std::find(this->validMovements[curr].begin(), 
            this->validMovements[curr].end(),
            target) == this->validMovements[curr].end();
    if(moveNotValid){
        return false;
    }
    
    if(promote <= PAWN || promote >= KING){
        return false;
    }
    char currPiece = this->board[curr];
    this->board[target] = this->board[curr];
    this->board[curr] = 0;

    if(currPiece % 10 == PAWN){
        if(std::abs(curr - target) == 16){
            if(this->whiteTurn){
                this->whitePassant = target;
            }
            else{
                this->blackPassant = target;
            }
        }
        else{
            if(this->whiteTurn){
                if(target + 8 == this->blackPassant){
                    this->board[this->blackPassant] = 0;
                }
                else if(target < 8){
                    this->board[target] = promote + 10;
                }
            }
            else{
                if(target - 8 == this->whitePassant){
                    this->board[this->whitePassant] = 0;
                }
                else if(target >=56){
                    this->board[target] = promote;
                }
            }
        }
    }
    if(curr == 0 || target == 0){
        this->blackQueenSideCastle = false;
    }
    else if(curr == 7 || target == 7){
        this->blackKingSideCastle = false;
    }
    else if(curr == 63 || target == 63){
        this->whiteKingSideCastle = false;
    }
    else if(curr == 56 || target == 56){
        this->whiteQueenSideCastle = false;
    }

    if(currPiece % 10 == KING){
        if(this->whiteTurn){
            this->whiteKingSideCastle = false;
            this->whiteQueenSideCastle = false;
            if(curr - target == 2){
                this->board[59] = ROOK + 10;
                this->board[56] = 0;
            }
            else if(target - curr == 2){
                this->board[61] = ROOK + 10;
                this->board[63] = 0;
            }
            this->whiteKingPos = target;
        }
        else{
            this->blackKingSideCastle = false;
            this->blackQueenSideCastle = false;
            if(curr - target == 2){
                this->board[3] = ROOK;
                this->board[0] = 0;
            }
            else if(target - curr == 2){
                this->board[5] = ROOK;
                this->board[7] = 0;
            }
            this->blackKingPos = target;
        }
    }
    this->switchTurn();

    return true;
}

bool ChessBoard::pieceAtPosition(int piece, std::pair<int, int> pos){
    if(pos.first < 0 || pos.second < 0 ||
            pos.first >= 8 || pos.second >= 8){
        return false;
    }
    if(this->board[pos.first * 8 + pos.second] == piece) return true;
    return false;
}

bool ChessBoard::boardInCheck(){
    int kingPos = this->blackKingPos;
    if(this->whiteTurn){
        kingPos = this->whiteKingPos;
    }

    int x = kingPos / 8;
    int y = kingPos % 8;
    bool checkedByPawn;
    int rook = ROOK;
    int knight = KNIGHT;
    int bishop = BISHOP;
    int queen = QUEEN;
    int king = KING;
    if(!this->whiteTurn){
        rook += 10;
        knight += 10;
        bishop += 10;
        queen += 10;
        king += 10;
        checkedByPawn = 
            pieceAtPosition(PAWN + 10, {x + 1, y + 1}) ||
            pieceAtPosition(PAWN + 10, {x + 1, y - 1});
    }
    else{
        checkedByPawn = 
            pieceAtPosition(PAWN, {x - 1, y + 1}) ||
            pieceAtPosition(PAWN, {x - 1, y - 1});
    }
    if(checkedByPawn) return true;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            if(pieceAtPosition(king, {x + i, y + j})){
                return true;
            }
        }
    }
    bool checkedByKnight =
        pieceAtPosition(knight, {x + 2, y + 1}) ||
        pieceAtPosition(knight, {x + 2, y - 1}) ||
        pieceAtPosition(knight, {x + 1, y + 2}) ||
        pieceAtPosition(knight, {x + 1, y - 2}) ||
        pieceAtPosition(knight, {x - 1, y + 2}) ||
        pieceAtPosition(knight, {x - 1, y - 2}) ||
        pieceAtPosition(knight, {x - 2, y + 1}) ||
        pieceAtPosition(knight, {x - 2, y - 1});
    if(checkedByKnight) return true;
        
    for(int i = x - 1; i >= 0; i--){
        int pos = i * 8 + y;
        if(this->board[pos] == rook ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int i = x + 1; i < 8; i++){
        int pos = i * 8 + y;
        if(this->board[pos] == rook ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int j = y + 1; j < 8; j++){
        int pos = x * 8 + j;
        if(this->board[pos] == rook ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int j = y - 1; j >= 0; j--){
        int pos = x * 8 + j;
        if(this->board[pos] == rook ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }

    for(int i = 1; x + i < 8 && y + i < 8; i++){
        int pos = (x + i) * 8 + (y + i);
        if(this->board[pos] == bishop ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int i = 1; x + i < 8 && y - i >= 0; i++){
        int pos = (x + i) * 8 + (y - i);
        if(this->board[pos] == bishop ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int i = 1; x - i >= 0 && y + i < 8; i++){
        int pos = (x - i) * 8 + (y + i);
        if(this->board[pos] == bishop ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }
    for(int i = 1; x - i >= 0 && y - i >= 0; i++){
        int pos = (x - i) * 8 + (y - i);
        if(this->board[pos] == bishop ||
                this->board[pos] == queen){
            return true;
        }
        else if(this->board[pos] != 0){
            break;
        }
    }

    return false;
}

void ChessBoard::switchTurn(){
    if(!this->gameRunning){
        std::cout << "Game Not Started!!!" << std::endl;
        return;
    }
    this->whiteTurn = !this->whiteTurn;
    if(this->whiteTurn){
        this->whitePassant = -64;
    }
    else{
        this->blackPassant = -64;
    }
    this->findValidMovements();
//    std::cout << "=======" << std::endl;
//    for(auto i : this->validMovements){
//        std::cout << i.first << ": " << std::endl << "\t";
//        for(int n : i.second){
//            std::cout <<  n << ", ";
//        }
//        std::cout << std::endl;
//    }
}

bool ChessBoard::gameIsRunning(){
    return this->gameRunning;
}

bool ChessBoard::isSelectable(std::pair<int, int> piece){
    int idx = piece.first * 8 + piece.second;
    if(idx < 0 || idx >= 64){
        return false;
    }
    bool isOwnPiece = (this->board[idx] > 10) == this->whiteTurn;
    bool notEmpty = this->board[idx] != 0;
    return isOwnPiece && notEmpty;
}

bool ChessBoard::compare(ChessBoard board2){
    std::vector<char> secondBoard = board2.getBoard();
    for(int i = 0; i < secondBoard.size(); i++){
        if(this->board[i] != secondBoard[i]){
            return false;
        }
    }
    return true;
}

void ChessBoard::printBoard(){
    std::string board = "";
    for(int i = 0; i < this->board.size(); i += 8){
        for(int j = 0; j < 8; j++){
            char curr;
            if(this->board[i + j] > 10){
                curr = pieceName[this->board[i + j] - 10];
                curr = tolower(curr);
            }
            else{
                curr = pieceName[this->board[i + j]];
            }
            board += curr;
            board += ", ";
        }
        board += '\n';
    }
    std::cout << board << std::endl;
}

std::vector<char> ChessBoard::getGameBoard(){
    return this->board;
}

std::vector<char> ChessBoard::getBoard(){
    std::vector<char> copy;
    for(int i = 0; i < this->board.size(); i++){
        if(this->board[i] < 10){
            copy.push_back(pieceName[this->board[i]]);
        }
        else{
            copy.push_back(tolower(pieceName[this->board[i] - 10]));
        }
    }
    return copy;
}

 //////////////// CHECK MOVEMENTS ////////////////////////
bool ChessBoard::checkPawnMovement(
    std::pair<int, int> from, std::pair<int, int> to
){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;

    int rowDiff;
    int colDiff = std::abs(from.second - to.second);

    if(this->whiteTurn){
        bool movingBackward = from.first <= to.first;
        if(movingBackward){
            return false;
        }
        rowDiff = from.first - to.first;
    }
    else{
        bool movingBackward = to.first <= from.first;
        if(movingBackward){
            return false;
        }
        rowDiff = to.first - from.first;
    }
    
    switch(rowDiff){
        case 1: {
            if(colDiff == 0){
                return this->board[target] == 0;
            }
            if(colDiff == 1){
                if(this->whiteTurn){
                    if(from.first == 3){
                        if(this->blackPassant == target + 8){
                            return true;
                        }
                    }
                }
                else{
                    if(from.first == 4){
                        if(this->whitePassant == target - 8){
                            return true;
                        }
                    }
                }
                bool enemyTarget = 
                    this->whiteTurn == (this->board[target] < 10);
                if(!enemyTarget || this->board[target] == 0){
                    if(this->whiteTurn){
                        if(from.first == 3 &&
                                (this->blackPassant == curr - 1 || 
                                 this->blackPassant == curr + 1)){

                        }
                    }
                    return false;
                }
            }
            else{
                return false;
            }
            break;
        }
        case 2: {
            if(colDiff != 0) return false;
            if(this->board[target] != 0){
                return false;
            }
            int middleRow = (from.first + to.first)/2;
            int middleIdx = 8 * middleRow + to.second;
            bool somethingInBetween = this->board[middleIdx] != 0;
            int validStartRow = 1 + 5 * this->whiteTurn;
            if(from.first != validStartRow || somethingInBetween){
                return false;
            }
            break;
        }
        default:
            return false;
    }
    return true;
}

bool ChessBoard::checkRookMovement(
        std::pair<int, int> from, std::pair<int, int> to
){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;
    bool isStraightLine = from.first == to.first || from.second == to.second;
    if(!isStraightLine){
        return false;
    }
    bool validTarget = 
        this->board[target] == 0 ||
        (
            this->whiteTurn == (this->board[target] < 10)
        );
    if(!validTarget){
        return false;
    }
    int start, end;
    if(from.first == to.first){
        if(from.second > to.second){
            start = to.second;
            end = from.second;
        }
        else{
            start = from.second;
            end = to.second;
        }
        for(int i = start + 1; i < end; i++){
            if(this->board[from.first * 8 + i] != 0){
                return false;
            }
        }
    }
    else{
        if(from.first > to.first){
            start = to.first;
            end = from.first;
        }
        else{
            start = from.first;
            end = to.first;
        }
        for(int i = start + 1; i < end; i++){
            if(this->board[i * 8 + from.second] != 0){
                return false;
            }
        }
    }

    return true;
}

bool ChessBoard::checkKnightMovement(
        std::pair<int, int> from, std::pair<int, int> to
    ){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;
    bool validTarget = 
        this->board[target] == 0 ||
        (
            this->whiteTurn == (this->board[target] < 10)
        );
    if(!validTarget){
        return false;
    }
    int rowDiff = std::abs(from.first - to.first);
    int colDiff = std::abs(from.second - to.second);
//    if(std::abs(rowDiff - colDiff) != 1){
//        return false;
//    }
    if(rowDiff != 1 && colDiff != 1 ||
            rowDiff != 2 && colDiff != 2){
        return false;
    }

    return true;
}

bool ChessBoard::checkBishopMovement(
        std::pair<int, int> from, std::pair<int, int> to
    ){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;
    bool validTarget = 
        this->board[target] == 0 ||
        (
            this->whiteTurn == (this->board[target] < 10)
        );
    if(!validTarget){
        return false;
    }
    int rowDiff = from.first - to.first;
    int colDiff = from.second - to.second;
    bool diagnalMovement = std::abs(rowDiff) == std::abs(colDiff);
    if(!diagnalMovement){
        return false;
    }
    
    int start, end;
    int increment;
    if(rowDiff < 0){
        if(colDiff < 0){
            increment = 9;
        }
        else{
            increment = 7;
        }
        start = curr + increment;
        end = target;
    }
    else{
        if(colDiff < 0){
            increment = 7;
        }
        else{
            increment = 9;
        }
        start = target + increment;
        end = curr;
    }
    
    for(int i = start; i < end; i += increment){
        if(this->board[i] != 0){
            return false;
        }
    }
    return true;
}

bool ChessBoard::checkQueenMovement(
        std::pair<int, int> from, std::pair<int, int> to
    ){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;

    int rowDiff = from.first - to.first;
    int colDiff = from.second - to.second;

    bool straightMovement = (
                rowDiff == 0 || colDiff == 0
            );
    bool diagonalMovement = (
                std::abs(rowDiff) ==
                std::abs(colDiff)
            );
    if(!straightMovement && !diagonalMovement){
        return false;
    }
    
    bool validTarget = 
        this->board[target] == 0 ||
        (
            this->whiteTurn == (this->board[target] < 10)
        );
    if(!validTarget){
        return false;
    }

    int start, end;
    int increment;
    if(straightMovement){
        if(rowDiff == 0){
            increment = 1;
        }
        else{
            increment = 8;
        }

        if(rowDiff > 0 || colDiff > 0){
            start = target + increment;
            end = curr;
        }
        else{
            start = curr + increment;
            end = target;
        }
    }
    else{
        if(rowDiff > 0){
            if(colDiff > 0){
                increment = 9;
            }
            else{
                increment = 7;
            }
            start = target + increment;
            end = curr;
        }
        else{
            if(colDiff > 0){
                increment = 7;
            }
            else{
                increment = 9;
            }
            start = curr + increment;
            end = target;
        }
    }
    for(int i = start; i < end; i += increment){
        if(this->board[i] != 0){
            return false;
        }
    }
    return true;
}

bool ChessBoard::checkKingMovement(
        std::pair<int, int> from, std::pair<int, int> to
    ){
    int curr = from.first * 8 + from.second;
    int target = to.first * 8 + to.second;

    int rowDiff = std::abs(from.first - to.first);
    int colDiff = std::abs(from.second - to.second);

    if(rowDiff > 1){
        return false;
    }
    if(rowDiff == 0 && colDiff == 2){
        if(from.second > to.second){
            if(this->whiteTurn){
                if(this->whiteQueenSideCastle){
                    if(
                            this->board[59] != 0 ||
                            this->board[58] != 0 ||
                            this->board[57] != 0){
                        return false;
                    }
                    if(boardInCheck()) return false;
                    if(!validateMove(60, 59)) return false;
                    if(!validateMove(60, 58)) return false;
                    return true;
                }
            }
            else{
                if(this->blackQueenSideCastle){
                    if(
                            this->board[3] != 0 ||
                            this->board[2] != 0 ||
                            this->board[1] != 0){
                        return false;
                    }
                    if(boardInCheck()) return false;
                    if(!validateMove(4, 3)) return false;
                    if(!validateMove(4, 2)) return false;
                    return true;
                }
            }
        }
        else{
            if(this->whiteTurn){
                if(this->whiteKingSideCastle){
                    if(
                            this->board[61] != 0 ||
                            this->board[62] != 0){
                        return false;
                    }
                    if(boardInCheck()) return false;
                    if(!validateMove(60, 61)) return false;
                    if(!validateMove(60, 62)) return false;
                    return true;
                }
            }
            else{
                if(this->blackKingSideCastle){
                    if(
                            this->board[5] != 0 ||
                            this->board[6] != 0){
                        return false;
                    }
                    if(boardInCheck()) return false;
                    if(!validateMove(4, 5)) return false;
                    if(!validateMove(4, 6)) return false;
                    return true;
                }
            }
        }
    }
    if(colDiff > 1){
        return false;
    }

    bool validTarget = 
        this->board[target] == 0 ||
        (
            this->whiteTurn == (this->board[target] < 10)
        );
    if(!validTarget){
        return false;
    }
    return true;
}

bool ChessBoard::checkPawnMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkPawnMovement({from / 8, from % 8}, to);
}
bool ChessBoard::checkRookMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkRookMovement({from / 8, from % 8}, to);
}
bool ChessBoard::checkKnightMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkKnightMovement({from/8, from%8}, to);
}
bool ChessBoard::checkBishopMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkBishopMovement({from/8, from%8}, to);
}
bool ChessBoard::checkQueenMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkQueenMovement({from/8, from%8}, to);
}
bool ChessBoard::checkKingMovement(int from, std::pair<int, int> to){
    if(to.first < 0 || to.second < 0 || 
            to.first >= 8 || to.second >= 8){
        return false;
    }
    return this->checkKingMovement({from/8, from%8}, to);
}

bool ChessBoard::checkPawnMovement(int from, int to){
    return this->checkPawnMovement({from / 8, from % 8}, {to / 8, to % 8});
}
bool ChessBoard::checkRookMovement(int from, int to){
    return this->checkRookMovement({from / 8, from % 8}, {to / 8, to % 8});
}
bool ChessBoard::checkKnightMovement(int from, int to){
    return this->checkKnightMovement({from/8, from%8}, {to/8, to%8});
}
bool ChessBoard::checkBishopMovement(int from, int to){
    return this->checkBishopMovement({from/8, from%8}, {to/8, to%8});
}
bool ChessBoard::checkQueenMovement(int from, int to){
    return this->checkQueenMovement({from/8, from%8}, {to/8, to%8});
}
bool ChessBoard::checkKingMovement(int from, int to){
    return this->checkKingMovement({from/8, from%8}, {to/8, to%8});
}

void ChessBoard::setBoard(std::vector<char> newBoard, bool turn){
    this->whiteKingPos = -64;
    this->blackKingPos = -64;
    for(int i = 0; i < newBoard.size(); i++){
        char piece;
        switch(toupper(newBoard[i])){
            case '-':
                piece = 0;
                break;
            case 'A':
                piece = PAWN;
                break;
            case 'R':
                piece = ROOK;
                break;
            case 'N':
                piece = KNIGHT;
                break;
            case 'B':
                piece = BISHOP;
                break;
            case 'Q':
                piece = QUEEN;
                break;
            case 'K':{
                         if(newBoard[i] < 97){
                             this->blackKingPos = i;
                         }
                         else{
                             this->whiteKingPos = i;
                         }
                         piece = KING;
                         break;
                     }
        }
        if(newBoard[i] >= 97){
            piece += 10;
        }
        newBoard[i] = piece;
    }

    this->whiteTurn = turn;
    this->board = newBoard;
    this->gameRunning = false;
    this->findValidMovements();
    this->whitePassant = -64;
    this->blackPassant = -64;

    this->blackQueenSideCastle = this->board[0] == ROOK;
    this->whiteQueenSideCastle = this->board[7] == ROOK;
    this->blackKingSideCastle = this->board[56] == ROOK + 10;
    this->whiteKingSideCastle = this->board[63] == ROOK + 10;
    if(this->board[4] != KING){
        this->blackQueenSideCastle = false;
        this->blackKingSideCastle = false;
    }
    if(this->board[60] != KING + 10){
        this->whiteKingSideCastle = false;
        this->whiteQueenSideCastle = false;
    }
}

ChessBoard& ChessBoard::operator=(ChessBoard board){
    this->whiteTurn = board.whiteTurn;
    this->board = board.board;
    this->whiteKingPos = board.whiteKingPos;
    this->blackKingPos = board.blackKingPos;
    this->gameRunning = board.gameRunning;
    this->findValidMovements();
    this->whitePassant = board.whitePassant;
    this->blackPassant = board.blackPassant;
    this->blackQueenSideCastle = board.blackQueenSideCastle;
    this->whiteQueenSideCastle = board.whiteQueenSideCastle;
    this->blackKingSideCastle = board.blackKingSideCastle;
    this->whiteKingSideCastle = board.whiteKingSideCastle;
    return *this;
}


