#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <TFT_eSPI.h>
#include "Board.h"

class DisplayManager {
public:
    TFT_eSPI tft;

    DisplayManager();

    void begin();

    void drawBoard(Board &board);

    void drawSquare(
        int row,
        int col,
        uint16_t color
    );

    void drawPiece(
        int row,
        int col,
        char piece
    );

    void highlightSquare(
        int row,
        int col
    );

    void showMessage(
        const char* msg
    );
};

#endif
