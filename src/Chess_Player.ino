#include "Board.h"
#include "MoveGenerator.h"
#include "DisplayManager.h"

Board board;
MoveGenerator moveGen;
DisplayManager display;

void setup() {

    Serial.begin(115200);

    display.begin();

    board.resetBoard();

    display.drawBoard(board);
}

void loop() {

}
