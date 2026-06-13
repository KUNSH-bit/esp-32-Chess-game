#include "Board.h"

Board::Board() {
    resetBoard();
}

void Board::resetBoard() {

    char initial[8][8] = {
        {'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}
    };

    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            board[r][c] = initial[r][c];
        }
    }

    whiteTurn = true;
}

char Board::getPiece(int row, int col) {
    return board[row][col];
}

void Board::setPiece(int row, int col, char piece) {
    board[row][col] = piece;
}

bool Board::isWhitePiece(char piece) {
    return piece >= 'A' && piece <= 'Z';
}

bool Board::isBlackPiece(char piece) {
    return piece >= 'a' && piece <= 'z';
}

bool Board::inBounds(int row, int col) {
    return row >= 0 &&
           row < 8 &&
           col >= 0 &&
           col < 8;
}

bool Board::movePiece(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    if(!inBounds(fromRow, fromCol))
        return false;

    if(!inBounds(toRow, toCol))
        return false;

    char piece = board[fromRow][fromCol];

    board[toRow][toCol] = piece;

    board[fromRow][fromCol] = ' ';

    whiteTurn = !whiteTurn;

    return true;
}

void Board::printBoard() {

    for(int r = 0; r < 8; r++) {

        for(int c = 0; c < 8; c++) {

            Serial.print(board[r][c]);
            Serial.print(' ');
        }

        Serial.println();
    }

    Serial.println();
}
