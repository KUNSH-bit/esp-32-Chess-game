#include "Board.h"
#include "MoveGenerator.h"

MoveGenerator moveGen;

Board board;

void setup() {

    Serial.begin(115200);

    board.printBoard();
}

void loop() {

}
