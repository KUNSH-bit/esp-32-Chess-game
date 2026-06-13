#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Board.h"

class MoveGenerator {
public:

    bool isLegalMove(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

private:

    bool validatePawn(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool validateRook(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool validateKnight(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool validateBishop(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool validateQueen(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );

    bool validateKing(
        Board &board,
        int fromRow,
        int fromCol,
        int toRow,
        int toCol
    );
};

#endif
