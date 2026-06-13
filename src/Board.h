#ifndef BOARD_H
#define BOARD_H

class Board {
public:
    char board[8][8];

    bool whiteTurn;

    Board();

    void resetBoard();

    char getPiece(int row, int col);

    void setPiece(int row, int col, char piece);

    bool isWhitePiece(char piece);

    bool isBlackPiece(char piece);

    bool movePiece(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool inBounds(int row, int col);

    void printBoard();
};

#endif
