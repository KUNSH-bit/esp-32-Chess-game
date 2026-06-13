#include "MoveGenerator.h"

bool MoveGenerator::isLegalMove(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    char piece = board.getPiece(fromRow, fromCol);

    if(piece == ' ')
        return false;

    switch(piece) {

        case 'P':
        case 'p':
            return validatePawn(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );

        case 'R':
        case 'r':
            return validateRook(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );

        case 'N':
        case 'n':
            return validateKnight(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );

        case 'B':
        case 'b':
            return validateBishop(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );

        case 'Q':
        case 'q':
            return validateQueen(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );

        case 'K':
        case 'k':
            return validateKing(
                board,
                fromRow,
                fromCol,
                toRow,
                toCol
            );
    }

    return false;
}

bool MoveGenerator::validatePawn(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    char piece = board.getPiece(
        fromRow,
        fromCol
    );

    int direction =
        board.isWhitePiece(piece) ? -1 : 1;

    if(
        fromCol == toCol &&
        toRow == fromRow + direction &&
        board.getPiece(toRow, toCol) == ' '
    )
        return true;

    return false;
}

bool MoveGenerator::validateRook(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    return
        fromRow == toRow ||
        fromCol == toCol;
}

bool MoveGenerator::validateKnight(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return
        (dr == 2 && dc == 1) ||
        (dr == 1 && dc == 2);
}

bool MoveGenerator::validateBishop(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    return
        abs(toRow - fromRow) ==
        abs(toCol - fromCol);
}

bool MoveGenerator::validateQueen(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    return
        validateRook(
            board,
            fromRow,
            fromCol,
            toRow,
            toCol
        )
        ||
        validateBishop(
            board,
            fromRow,
            fromCol,
            toRow,
            toCol
        );
}

bool MoveGenerator::validateKing(
    Board &board,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) {

    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return dr <= 1 && dc <= 1;
}
