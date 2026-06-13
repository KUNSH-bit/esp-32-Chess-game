#include "DisplayManager.h"

#define CELL_SIZE 16

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

void DisplayManager::drawSquare(
    int row,
    int col,
    uint16_t color
) {

    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;

    tft.fillRect(
        x,
        y,
        CELL_SIZE,
        CELL_SIZE,
        color
    );
}

void DisplayManager::drawPiece(
    int row,
    int col,
    char piece
) {

    int x = col * CELL_SIZE + 4;
    int y = row * CELL_SIZE + 4;

    tft.setTextColor(TFT_RED);
    tft.drawChar(
        piece,
        x,
        y,
        2
    );
}

void DisplayManager::drawBoard(
    Board &board
) {

    for(int r=0;r<8;r++) {

        for(int c=0;c<8;c++) {

            uint16_t color =
                ((r+c)%2)
                ? TFT_WHITE
                : TFT_BLACK;

            drawSquare(
                r,
                c,
                color
            );

            drawPiece(
                r,
                c,
                board.board[r][c]
            );
        }
    }
}

void DisplayManager::highlightSquare(
    int row,
    int col
) {

    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;

    tft.drawRect(
        x,
        y,
        CELL_SIZE,
        CELL_SIZE,
        TFT_YELLOW
    );
}

void DisplayManager::showMessage(
    const char* msg
) {

    tft.fillRect(
        0,
        130,
        160,
        30,
        TFT_BLUE
    );

    tft.setCursor(5,135);
    tft.setTextColor(TFT_WHITE);
    tft.print(msg);
}
