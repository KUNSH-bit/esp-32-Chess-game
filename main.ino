#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS 5
#define TFT_DC 2

#define BTN_UP 32
#define BTN_DOWN 33
#define BTN_LEFT 25
#define BTN_RIGHT 26
#define BTN_SELECT 27
#define BTN_CANCEL 14

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

const int BOARD_X = 4;
const int BOARD_Y = 24;
const int CELL = 24;
const int BOARD_SIZE = 192;
const int PANEL_X = 200;

char board[8][8];

char historyBoard[301][8][8];
bool historyWhiteTurn[301];
bool historyWK[301];
bool historyWQ[301];
bool historyBK[301];
bool historyBQ[301];
int historyEPX[301];
int historyEPY[301];
int historyCount = 0;

bool whiteTurn = true;
bool whiteKingSide = true;
bool whiteQueenSide = true;
bool blackKingSide = true;
bool blackQueenSide = true;

int enPassantX = -1;
int enPassantY = -1;

int cursorX = 4;
int cursorY = 6;

int selectedX = -1;
int selectedY = -1;

bool gameRunning = false;
bool vsAI = false;
int aiLevel = 1;

bool promotionActive = false;
int promotionX = -1;
int promotionY = -1;

bool menuActive = true;
bool aiMenuActive = false;
int menuChoice = 0;

unsigned long lastButtonTime = 0;

const unsigned long BUTTON_DELAY = 150;

struct Move {
  int sx;
  int sy;
  int dx;
  int dy;
  char promotion;
  bool enPassant;
  bool castle;
};

struct Snapshot {
  char b[8][8];
  bool whiteTurn;
  bool wks;
  bool wqs;
  bool bks;
  bool bqs;
  int epX;
  int epY;
};

bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW) {
    if (millis() - lastButtonTime > BUTTON_DELAY) {
      lastButtonTime = millis();
      return true;
    }
  }
  return false;
}

bool inside(int x, int y) { return x >= 0 && x < 8 && y >= 0 && y < 8; }

bool whitePiece(char p) { return p >= 'A' && p <= 'Z'; }

bool blackPiece(char p) { return p >= 'a' && p <= 'z'; }

bool sameColor(char a, char b) {
  if (a == '.' || b == '.')
    return false;
  return whitePiece(a) == whitePiece(b);
}

void copyBoard(char source[8][8], char target[8][8]) {
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
      target[y][x] = source[y][x];
}

void saveSnapshot() {
  if (historyCount >= 300)
    return;

  copyBoard(board, historyBoard[historyCount]);

  historyWhiteTurn[historyCount] = whiteTurn;
  historyWK[historyCount] = whiteKingSide;
  historyWQ[historyCount] = whiteQueenSide;
  historyBK[historyCount] = blackKingSide;
  historyBQ[historyCount] = blackQueenSide;
  historyEPX[historyCount] = enPassantX;
  historyEPY[historyCount] = enPassantY;

  historyCount++;
}

void undoMove() {
  if (historyCount <= 0)
    return;

  historyCount--;

  copyBoard(historyBoard[historyCount], board);

  whiteTurn = historyWhiteTurn[historyCount];
  whiteKingSide = historyWK[historyCount];
  whiteQueenSide = historyWQ[historyCount];
  blackKingSide = historyBK[historyCount];
  blackQueenSide = historyBQ[historyCount];
  enPassantX = historyEPX[historyCount];
  enPassantY = historyEPY[historyCount];

  selectedX = -1;
  selectedY = -1;
  promotionActive = false;
}

void setupBoard() {
  const char initial[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                              {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                              {'.', '.', '.', '.', '.', '.', '.', '.'},
                              {'.', '.', '.', '.', '.', '.', '.', '.'},
                              {'.', '.', '.', '.', '.', '.', '.', '.'},
                              {'.', '.', '.', '.', '.', '.', '.', '.'},
                              {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                              {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

  copyBoard((char (*)[8])initial, board);

  whiteTurn = true;

  whiteKingSide = true;
  whiteQueenSide = true;
  blackKingSide = true;
  blackQueenSide = true;

  enPassantX = -1;
  enPassantY = -1;

  selectedX = -1;
  selectedY = -1;

  cursorX = 4;
  cursorY = 6;

  historyCount = 0;
  promotionActive = false;
}

void findKing(bool white, int &kx, int &ky) {
  kx = -1;
  ky = -1;

  char king = white ? 'K' : 'k';

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (board[y][x] == king) {
        kx = x;
        ky = y;
        return;
      }
    }
  }
}

bool squareAttacked(int x, int y, bool byWhite) {
  int dir = byWhite ? -1 : 1;
  char pawn = byWhite ? 'P' : 'p';

  for (int dx = -1; dx <= 1; dx += 2) {
    int px = x + dx;
    int py = y + dir;

    if (inside(px, py) && board[py][px] == pawn)
      return true;
  }

  const int knightMoves[8][2] = {{1, 2},   {2, 1},   {2, -1}, {1, -2},
                                 {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

  char knight = byWhite ? 'N' : 'n';

  for (int i = 0; i < 8; i++) {
    int nx = x + knightMoves[i][0];
    int ny = y + knightMoves[i][1];

    if (inside(nx, ny) && board[ny][nx] == knight)
      return true;
  }

  char bishop = byWhite ? 'B' : 'b';
  char rook = byWhite ? 'R' : 'r';
  char queen = byWhite ? 'Q' : 'q';
  char king = byWhite ? 'K' : 'k';

  const int diagonals[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (int d = 0; d < 4; d++) {
    int nx = x + diagonals[d][0];
    int ny = y + diagonals[d][1];

    while (inside(nx, ny)) {
      char p = board[ny][nx];

      if (p != '.') {
        if (p == bishop || p == queen)
          return true;
        break;
      }

      nx += diagonals[d][0];
      ny += diagonals[d][1];
    }
  }

  const int straight[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  for (int d = 0; d < 4; d++) {
    int nx = x + straight[d][0];
    int ny = y + straight[d][1];

    while (inside(nx, ny)) {
      char p = board[ny][nx];

      if (p != '.') {
        if (p == rook || p == queen)
          return true;
        break;
      }

      nx += straight[d][0];
      ny += straight[d][1];
    }
  }

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx == 0 && dy == 0)
        continue;

      int nx = x + dx;
      int ny = y + dy;

      if (inside(nx, ny) && board[ny][nx] == king)
        return true;
    }
  }

  return false;
}

bool inCheck(bool white) {
  int kx, ky;
  findKing(white, kx, ky);

  if (kx == -1)
    return true;

  return squareAttacked(kx, ky, !white);
}

void addMove(Move moves[], int &count, int sx, int sy, int dx, int dy,
             char promotion = 0, bool ep = false, bool castle = false) {
  if (count >= 300)
    return;

  moves[count].sx = sx;
  moves[count].sy = sy;
  moves[count].dx = dx;
  moves[count].dy = dy;
  moves[count].promotion = promotion;
  moves[count].enPassant = ep;
  moves[count].castle = castle;

  count++;
}

void generatePseudoMoves(bool white, Move moves[], int &count) {
  count = 0;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      char p = board[y][x];

      if (p == '.')
        continue;
      if (whitePiece(p) != white)
        continue;

      char type = p;

      if (type == 'P' || type == 'p') {
        int dir = white ? -1 : 1;
        int startY = white ? 6 : 1;
        int promotionY = white ? 0 : 7;

        int ny = y + dir;

        if (inside(x, ny) && board[ny][x] == '.') {
          if (ny == promotionY) {
            addMove(moves, count, x, y, x, ny, 'Q');
            addMove(moves, count, x, y, x, ny, 'R');
            addMove(moves, count, x, y, x, ny, 'B');
            addMove(moves, count, x, y, x, ny, 'N');
          } else {
            addMove(moves, count, x, y, x, ny);
          }

          if (y == startY && board[y + dir * 2][x] == '.') {
            addMove(moves, count, x, y, x, y + dir * 2);
          }
        }

        for (int dx = -1; dx <= 1; dx += 2) {
          int nx = x + dx;

          if (!inside(nx, ny))
            continue;

          if (board[ny][nx] != '.' && whitePiece(board[ny][nx]) != white) {
            if (ny == promotionY) {
              addMove(moves, count, x, y, nx, ny, 'Q');
              addMove(moves, count, x, y, nx, ny, 'R');
              addMove(moves, count, x, y, nx, ny, 'B');
              addMove(moves, count, x, y, nx, ny, 'N');
            } else {
              addMove(moves, count, x, y, nx, ny);
            }
          }

          if (nx == enPassantX && ny == enPassantY) {
            addMove(moves, count, x, y, nx, ny, 0, true);
          }
        }
      }

      if (type == 'N' || type == 'n') {
        const int jumps[8][2] = {{1, 2},   {2, 1},   {2, -1}, {1, -2},
                                 {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

        for (int i = 0; i < 8; i++) {
          int nx = x + jumps[i][0];
          int ny = y + jumps[i][1];

          if (!inside(nx, ny))
            continue;

          if (board[ny][nx] == '.' || whitePiece(board[ny][nx]) != white)
            addMove(moves, count, x, y, nx, ny);
        }
      }

      if (type == 'B' || type == 'b' || type == 'R' || type == 'r' ||
          type == 'Q' || type == 'q') {
        int directions[8][2];
        int directionCount = 0;

        if (type == 'B' || type == 'b' || type == 'Q' || type == 'q') {
          directions[directionCount][0] = 1;
          directions[directionCount++][1] = 1;
          directions[directionCount][0] = 1;
          directions[directionCount++][1] = -1;
          directions[directionCount][0] = -1;
          directions[directionCount++][1] = 1;
          directions[directionCount][0] = -1;
          directions[directionCount++][1] = -1;
        }

        if (type == 'R' || type == 'r' || type == 'Q' || type == 'q') {
          directions[directionCount][0] = 1;
          directions[directionCount++][1] = 0;
          directions[directionCount][0] = -1;
          directions[directionCount++][1] = 0;
          directions[directionCount][0] = 0;
          directions[directionCount++][1] = 1;
          directions[directionCount][0] = 0;
          directions[directionCount++][1] = -1;
        }

        for (int d = 0; d < directionCount; d++) {
          int nx = x + directions[d][0];
          int ny = y + directions[d][1];

          while (inside(nx, ny)) {
            if (board[ny][nx] == '.') {
              addMove(moves, count, x, y, nx, ny);
            } else {
              if (whitePiece(board[ny][nx]) != white)
                addMove(moves, count, x, y, nx, ny);

              break;
            }

            nx += directions[d][0];
            ny += directions[d][1];
          }
        }
      }

      if (type == 'K' || type == 'k') {
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
              continue;

            int nx = x + dx;
            int ny = y + dy;

            if (!inside(nx, ny))
              continue;

            if (board[ny][nx] == '.' || whitePiece(board[ny][nx]) != white)
              addMove(moves, count, x, y, nx, ny);
          }
        }

        if (white && x == 4 && y == 7 && !inCheck(true)) {
          if (whiteKingSide && board[7][5] == '.' && board[7][6] == '.' &&
              board[7][7] == 'R' && !squareAttacked(5, 7, false) &&
              !squareAttacked(6, 7, false)) {
            addMove(moves, count, 4, 7, 6, 7, 0, false, true);
          }

          if (whiteQueenSide && board[7][1] == '.' && board[7][2] == '.' &&
              board[7][3] == '.' && board[7][0] == 'R' &&
              !squareAttacked(3, 7, false) && !squareAttacked(2, 7, false)) {
            addMove(moves, count, 4, 7, 2, 7, 0, false, true);
          }
        }

        if (!white && x == 4 && y == 0 && !inCheck(false)) {
          if (blackKingSide && board[0][5] == '.' && board[0][6] == '.' &&
              board[0][7] == 'r' && !squareAttacked(5, 0, true) &&
              !squareAttacked(6, 0, true)) {
            addMove(moves, count, 4, 0, 6, 0, 0, false, true);
          }

          if (blackQueenSide && board[0][1] == '.' && board[0][2] == '.' &&
              board[0][3] == '.' && board[0][0] == 'r' &&
              !squareAttacked(3, 0, true) && !squareAttacked(2, 0, true)) {
            addMove(moves, count, 4, 0, 2, 0, 0, false, true);
          }
        }
      }
    }
  }
}

Snapshot makeSnapshot() {
  Snapshot s;

  copyBoard(board, s.b);

  s.whiteTurn = whiteTurn;
  s.wks = whiteKingSide;
  s.wqs = whiteQueenSide;
  s.bks = blackKingSide;
  s.bqs = blackQueenSide;
  s.epX = enPassantX;
  s.epY = enPassantY;

  return s;
}

void restoreSnapshot(Snapshot &s) {
  copyBoard(s.b, board);

  whiteTurn = s.whiteTurn;
  whiteKingSide = s.wks;
  whiteQueenSide = s.wqs;
  blackKingSide = s.bks;
  blackQueenSide = s.bqs;
  enPassantX = s.epX;
  enPassantY = s.epY;
}

void updateCastlingRights(char moving, int sx, int sy, int dx, int dy,
                          char captured) {
  if (moving == 'K') {
    whiteKingSide = false;
    whiteQueenSide = false;
  }

  if (moving == 'k') {
    blackKingSide = false;
    blackQueenSide = false;
  }

  if (moving == 'R') {
    if (sx == 0 && sy == 7)
      whiteQueenSide = false;
    if (sx == 7 && sy == 7)
      whiteKingSide = false;
  }

  if (moving == 'r') {
    if (sx == 0 && sy == 0)
      blackQueenSide = false;
    if (sx == 7 && sy == 0)
      blackKingSide = false;
  }

  if (captured == 'R') {
    if (dx == 0 && dy == 7)
      whiteQueenSide = false;
    if (dx == 7 && dy == 7)
      whiteKingSide = false;
  }

  if (captured == 'r') {
    if (dx == 0 && dy == 0)
      blackQueenSide = false;
    if (dx == 7 && dy == 0)
      blackKingSide = false;
  }
}

void applyMove(const Move &m) {
  char moving = board[m.sy][m.sx];
  char captured = board[m.dy][m.dx];

  updateCastlingRights(moving, m.sx, m.sy, m.dx, m.dy, captured);

  board[m.sy][m.sx] = '.';

  if (m.enPassant) {
    int capturedY = whitePiece(moving) ? m.dy + 1 : m.dy - 1;
    board[capturedY][m.dx] = '.';
  }

  board[m.dy][m.dx] = moving;

  if (m.castle) {
    if (m.dx == 6) {
      board[m.dy][5] = board[m.dy][7];
      board[m.dy][7] = '.';
    } else if (m.dx == 2) {
      board[m.dy][3] = board[m.dy][0];
      board[m.dy][0] = '.';
    }
  }

  if ((moving == 'P' || moving == 'p') && abs(m.dy - m.sy) == 2) {
    enPassantX = m.sx;
    enPassantY = (m.sy + m.dy) / 2;
  } else {
    enPassantX = -1;
    enPassantY = -1;
  }

  if (m.promotion) {
    if (whitePiece(moving))
      board[m.dy][m.dx] = m.promotion;
    else
      board[m.dy][m.dx] = tolower(m.promotion);
  }

  whiteTurn = !whiteTurn;
}

bool legalMove(const Move &m, bool white) {
  Snapshot s = makeSnapshot();

  applyMove(m);

  bool legal = !inCheck(white);

  restoreSnapshot(s);

  return legal;
}

int generateLegalMoves(bool white, Move moves[], int &count) {
  Move pseudo[300];
  int pseudoCount = 0;

  generatePseudoMoves(white, pseudo, pseudoCount);

  count = 0;

  for (int i = 0; i < pseudoCount; i++) {
    if (legalMove(pseudo[i], white)) {
      moves[count++] = pseudo[i];
    }
  }

  return count;
}

bool isSameMove(const Move &a, const Move &b) {
  return a.sx == b.sx && a.sy == b.sy && a.dx == b.dx && a.dy == b.dy &&
         a.promotion == b.promotion && a.enPassant == b.enPassant &&
         a.castle == b.castle;
}

int pieceValue(char p) {
  switch (tolower(p)) {
  case 'p':
    return 100;
  case 'n':
    return 320;
  case 'b':
    return 330;
  case 'r':
    return 500;
  case 'q':
    return 900;
  case 'k':
    return 20000;
  }

  return 0;
}

int positionalValue(char p, int x, int y) {
  char type = tolower(p);
  int center =
      3 - min(abs(3 - x), abs(4 - x)) + 3 - min(abs(3 - y), abs(4 - y));

  if (type == 'p') {
    return whitePiece(p) ? (6 - y) * 8 : (y - 1) * 8;
  }

  if (type == 'n')
    return center * 8;
  if (type == 'b')
    return center * 5;
  if (type == 'q')
    return center * 2;

  return 0;
}

int evaluateBoard() {
  int score = 0;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      char p = board[y][x];

      if (p == '.')
        continue;

      int value = pieceValue(p) + positionalValue(p, x, y);

      if (whitePiece(p))
        score += value;
      else
        score -= value;
    }
  }

  return score;
}

int moveScore(const Move &m) {
  int score = 0;

  char captured = board[m.dy][m.dx];

  if (captured != '.')
    score += pieceValue(captured) * 10 - pieceValue(board[m.sy][m.sx]);

  if (m.enPassant)
    score += 900;

  if (m.promotion)
    score += pieceValue(m.promotion);

  if (m.castle)
    score += 50;

  return score;
}

int minimax(int depth, int alpha, int beta, bool maximizing) {
  Move moves[300];
  int count = 0;

  generateLegalMoves(maximizing, moves, count);

  if (count == 0) {
    if (inCheck(maximizing))
      return maximizing ? -1000000 - depth : 1000000 + depth;

    return 0;
  }

  if (depth == 0)
    return evaluateBoard();

  if (maximizing) {
    int best = -10000000;

    for (int i = 0; i < count; i++) {
      Snapshot s = makeSnapshot();

      applyMove(moves[i]);

      int score = minimax(depth - 1, alpha, beta, false);

      restoreSnapshot(s);

      best = max(best, score);
      alpha = max(alpha, best);

      if (beta <= alpha)
        break;
    }

    return best;
  } else {
    int best = 10000000;

    for (int i = 0; i < count; i++) {
      Snapshot s = makeSnapshot();

      applyMove(moves[i]);

      int score = minimax(depth - 1, alpha, beta, true);

      restoreSnapshot(s);

      best = min(best, score);
      beta = min(beta, best);

      if (beta <= alpha)
        break;
    }

    return best;
  }
}

Move chooseAIMove() {
  Move moves[300];
  int count = 0;

  generateLegalMoves(false, moves, count);

  Move fallback = moves[0];

  if (count <= 1)
    return fallback;

  if (aiLevel == 1) {
    return moves[random(count)];
  }

  if (aiLevel == 2) {
    int bestScore = -100000;

    for (int i = 0; i < count; i++) {
      int score = moveScore(moves[i]);

      if (score > bestScore) {
        bestScore = score;
        fallback = moves[i];
      }
    }

    return fallback;
  }

  int depth = 1;

  if (aiLevel == 3)
    depth = 2;
  if (aiLevel == 4)
    depth = 3;
  if (aiLevel == 5)
    depth = 3;

  int bestScore = 10000000;

  for (int i = 0; i < count; i++) {
    Snapshot s = makeSnapshot();

    applyMove(moves[i]);

    int score;

    if (aiLevel == 5) {
      score = minimax(depth - 1, -10000000, 10000000, true);
    } else {
      score = minimax(depth - 1, -10000000, 10000000, true);
    }

    restoreSnapshot(s);

    if (score < bestScore) {
      bestScore = score;
      fallback = moves[i];
    }
  }

  return fallback;
}

void drawPiece(char p, int x, int y) {
  if (p == '.')
    return;

  const char *names[] = {"", "K", "Q", "R", "B", "N", "P"};

  char upper = toupper(p);

  int index = 0;

  if (upper == 'K')
    index = 1;
  if (upper == 'Q')
    index = 2;
  if (upper == 'R')
    index = 3;
  if (upper == 'B')
    index = 4;
  if (upper == 'N')
    index = 5;
  if (upper == 'P')
    index = 6;

  tft.setTextSize(2);

  if (whitePiece(p))
    tft.setTextColor(ILI9341_WHITE);
  else
    tft.setTextColor(ILI9341_BLACK);

  tft.setCursor(x + 7, y + 4);
  tft.print(names[index]);
}

bool squareHasLegalMove(int x, int y) {
  Move moves[300];
  int count = 0;

  generateLegalMoves(whiteTurn, moves, count);

  for (int i = 0; i < count; i++) {
    if (moves[i].sx == selectedX && moves[i].sy == selectedY &&
        moves[i].dx == x && moves[i].dy == y)
      return true;
  }

  return false;
}

void drawBoard() {
  tft.fillScreen(ILI9341_BLACK);

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      uint16_t color = ((x + y) % 2 == 0) ? 0xC69C6D : 0x6B4423;

      tft.fillRect(BOARD_X + x * CELL, BOARD_Y + y * CELL, CELL, CELL, color);
    }
  }

  Move legalMoves[300];
  int legalCount = 0;

  if (selectedX != -1)
    generateLegalMoves(whiteTurn, legalMoves, legalCount);

  for (int i = 0; i < legalCount; i++) {
    if (legalMoves[i].sx == selectedX && legalMoves[i].sy == selectedY) {

      int cx = BOARD_X + legalMoves[i].dx * CELL + CELL / 2;
      int cy = BOARD_Y + legalMoves[i].dy * CELL + CELL / 2;

      if (board[legalMoves[i].dy][legalMoves[i].dx] == '.')
        tft.fillCircle(cx, cy, 3, ILI9341_GREEN);
      else
        tft.drawCircle(cx, cy, 8, ILI9341_GREEN);
    }
  }

  tft.drawRect(BOARD_X + cursorX * CELL, BOARD_Y + cursorY * CELL, CELL, CELL,
               ILI9341_YELLOW);

  if (selectedX != -1) {
    tft.drawRect(BOARD_X + selectedX * CELL, BOARD_Y + selectedY * CELL, CELL,
                 CELL, ILI9341_BLUE);
  }

  int kx, ky;
  findKing(whiteTurn, kx, ky);

  if (kx != -1 && inCheck(whiteTurn)) {
    tft.drawRect(BOARD_X + kx * CELL + 1, BOARD_Y + ky * CELL + 1, CELL - 2,
                 CELL - 2, ILI9341_RED);
  }

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      drawPiece(board[y][x], BOARD_X + x * CELL, BOARD_Y + y * CELL);
    }
  }

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);

  for (int x = 0; x < 8; x++) {
    tft.setCursor(BOARD_X + x * CELL + 9, BOARD_Y + BOARD_SIZE + 2);
    tft.print(char('a' + x));
  }

  for (int y = 0; y < 8; y++) {
    tft.setCursor(BOARD_X + BOARD_SIZE + 2, BOARD_Y + y * CELL + 8);
    tft.print(8 - y);
  }

  tft.fillRect(PANEL_X, 0, 120, 240, ILI9341_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(PANEL_X, 8);
  tft.print(whiteTurn ? "WHITE" : "BLACK");

  tft.setCursor(PANEL_X, 20);
  tft.print(vsAI ? "VS AI" : "2 PLAYER");

  if (vsAI) {
    tft.setCursor(PANEL_X, 32);
    tft.print("AI L");
    tft.print(aiLevel);
  }

  tft.setCursor(PANEL_X, 50);
  tft.print("SELECT");

  tft.setCursor(PANEL_X, 62);
  tft.print("MOVE");

  tft.setCursor(PANEL_X, 82);
  tft.print("CANCEL");

  tft.setCursor(PANEL_X, 94);
  tft.print("UNDO");

  tft.setCursor(PANEL_X, 116);
  tft.print("TURN");

  tft.setCursor(PANEL_X, 128);
  tft.print(whiteTurn ? "WHITE" : "BLACK");

  tft.setCursor(PANEL_X, 148);
  tft.print("UP/DOWN");

  tft.setCursor(PANEL_X, 160);
  tft.print("MOVE CURSOR");

  tft.setCursor(PANEL_X, 180);
  tft.print("LEFT/RIGHT");

  tft.setCursor(PANEL_X, 192);
  tft.print("MOVE CURSOR");
}

void drawMenu() {
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(3);
  tft.setCursor(48, 25);
  tft.print("CHESS");

  tft.setTextSize(2);

  tft.setCursor(40, 85);

  if (menuChoice == 0)
    tft.setTextColor(ILI9341_YELLOW);
  else
    tft.setTextColor(ILI9341_WHITE);

  tft.print("2 PLAYER");

  tft.setCursor(40, 125);

  if (menuChoice == 1)
    tft.setTextColor(ILI9341_YELLOW);
  else
    tft.setTextColor(ILI9341_WHITE);

  tft.print("VS AI");

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(40, 175);
  tft.print("UP/DOWN SELECT");

  tft.setCursor(40, 190);
  tft.print("SELECT = START");
}

void drawAIMenu() {
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(50, 15);
  tft.print("AI LEVEL");

  for (int i = 0; i < 5; i++) {
    tft.setCursor(55, 50 + i * 28);

    if (aiLevel == i + 1)
      tft.setTextColor(ILI9341_YELLOW);
    else
      tft.setTextColor(ILI9341_WHITE);

    tft.print("LEVEL ");
    tft.print(i + 1);
  }

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);

  tft.setCursor(50, 205);
  tft.print("UP/DOWN CHANGE");

  tft.setCursor(50, 220);
  tft.print("SELECT START");
}

void drawPromotionMenu() {
  tft.fillRect(40, 70, 160, 100, ILI9341_BLACK);
  tft.drawRect(40, 70, 160, 100, ILI9341_WHITE);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  tft.setCursor(65, 82);
  tft.print("PROMOTE");

  char options[4] = {'Q', 'R', 'B', 'N'};

  for (int i = 0; i < 4; i++) {
    tft.setCursor(58 + i * 35, 125);

    if (i == menuChoice)
      tft.setTextColor(ILI9341_YELLOW);
    else
      tft.setTextColor(ILI9341_WHITE);

    tft.print(options[i]);
  }
}

void drawGameOver(bool checkmate) {
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);

  tft.setCursor(45, 55);

  if (checkmate)
    tft.print("CHECKMATE");
  else
    tft.print("STALEMATE");

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);

  tft.setCursor(65, 100);

  if (checkmate) {
    tft.print(whiteTurn ? "BLACK WINS" : "WHITE WINS");
  } else {
    tft.print("DRAW");
  }

  tft.setCursor(55, 145);
  tft.print("SELECT = RESTART");
}

bool hasLegalMoves(bool white) {
  Move moves[300];
  int count = 0;

  generateLegalMoves(white, moves, count);

  return count > 0;
}

void performMove(const Move &m) {
  saveSnapshot();
  applyMove(m);

  selectedX = -1;
  selectedY = -1;

  promotionActive = false;

  drawBoard();
}

void selectSquare() {
  if (!gameRunning)
    return;

  if (vsAI && !whiteTurn)
    return;

  char p = board[cursorY][cursorX];

  if (selectedX == -1) {
    if (p != '.' && whitePiece(p) == whiteTurn) {
      selectedX = cursorX;
      selectedY = cursorY;
      drawBoard();
    }

    return;
  }

  Move moves[300];
  int count = 0;

  generateLegalMoves(whiteTurn, moves, count);

  for (int i = 0; i < count; i++) {
    if (moves[i].sx == selectedX && moves[i].sy == selectedY &&
        moves[i].dx == cursorX && moves[i].dy == cursorY) {

      if (moves[i].promotion) {
        promotionActive = true;
        promotionX = cursorX;
        promotionY = cursorY;
        menuChoice = 0;
        drawPromotionMenu();
      } else {
        performMove(moves[i]);
      }

      return;
    }
  }

  if (p != '.' && whitePiece(p) == whiteTurn) {
    selectedX = cursorX;
    selectedY = cursorY;
    drawBoard();
  } else {
    selectedX = -1;
    selectedY = -1;
    drawBoard();
  }
}

void checkGameState() {
  if (!gameRunning)
    return;

  Move moves[300];
  int count = 0;

  generateLegalMoves(whiteTurn, moves, count);

  if (count == 0) {
    gameRunning = false;
    drawGameOver(inCheck(whiteTurn));
  }
}

void aiTurn() {
  if (!vsAI || whiteTurn || !gameRunning)
    return;

  delay(150);

  Move move = chooseAIMove();

  saveSnapshot();
  applyMove(move);

  drawBoard();

  checkGameState();
}

void handlePromotion() {
  if (!promotionActive)
    return;

  if (buttonPressed(BTN_LEFT)) {
    menuChoice--;
    if (menuChoice < 0)
      menuChoice = 3;
    drawPromotionMenu();
  }

  if (buttonPressed(BTN_RIGHT)) {
    menuChoice++;
    if (menuChoice > 3)
      menuChoice = 0;
    drawPromotionMenu();
  }

  if (buttonPressed(BTN_SELECT)) {
    Move moves[300];
    int count = 0;

    generateLegalMoves(whiteTurn, moves, count);

    char selectedPiece[4] = {'Q', 'R', 'B', 'N'};

    for (int i = 0; i < count; i++) {
      if (moves[i].sx == selectedX && moves[i].sy == selectedY &&
          moves[i].dx == promotionX && moves[i].dy == promotionY &&
          moves[i].promotion == selectedPiece[menuChoice]) {

        performMove(moves[i]);
        checkGameState();
        return;
      }
    }
  }
}

void handleMenu() {
  if (aiMenuActive) {
    if (buttonPressed(BTN_UP)) {
      aiLevel--;
      if (aiLevel < 1)
        aiLevel = 5;
      drawAIMenu();
    }

    if (buttonPressed(BTN_DOWN)) {
      aiLevel++;
      if (aiLevel > 5)
        aiLevel = 1;
      drawAIMenu();
    }

    if (buttonPressed(BTN_SELECT)) {
      setupBoard();
      vsAI = true;
      gameRunning = true;
      menuActive = false;
      aiMenuActive = false;
      drawBoard();
    }

    if (buttonPressed(BTN_CANCEL)) {
      aiMenuActive = false;
      menuChoice = 0;
      drawMenu();
    }

    return;
  }

  if (buttonPressed(BTN_UP)) {
    menuChoice--;
    if (menuChoice < 0)
      menuChoice = 1;
    drawMenu();
  }

  if (buttonPressed(BTN_DOWN)) {
    menuChoice++;
    if (menuChoice > 1)
      menuChoice = 0;
    drawMenu();
  }

  if (buttonPressed(BTN_SELECT)) {
    if (menuChoice == 0) {
      setupBoard();
      vsAI = false;
      gameRunning = true;
      menuActive = false;
      drawBoard();
    } else {
      aiMenuActive = true;
      aiLevel = 1;
      drawAIMenu();
    }
  }
}

void handleGame() {
  if (!gameRunning) {
    if (buttonPressed(BTN_SELECT)) {
      setupBoard();
      gameRunning = true;
      menuActive = false;
      drawBoard();
    }

    return;
  }

  if (promotionActive) {
    handlePromotion();
    return;
  }

  if (buttonPressed(BTN_UP)) {
    cursorY--;
    if (cursorY < 0)
      cursorY = 7;
    drawBoard();
  }

  if (buttonPressed(BTN_DOWN)) {
    cursorY++;
    if (cursorY > 7)
      cursorY = 0;
    drawBoard();
  }

  if (buttonPressed(BTN_LEFT)) {
    cursorX--;
    if (cursorX < 0)
      cursorX = 7;
    drawBoard();
  }

  if (buttonPressed(BTN_RIGHT)) {
    cursorX++;
    if (cursorX > 7)
      cursorX = 0;
    drawBoard();
  }

  if (buttonPressed(BTN_SELECT)) {
    selectSquare();
    checkGameState();
  }

  if (buttonPressed(BTN_CANCEL)) {
    if (selectedX != -1) {
      selectedX = -1;
      selectedY = -1;
      drawBoard();
    } else if (historyCount > 0) {
      if (vsAI && !whiteTurn && historyCount >= 2) {
        undoMove();
        undoMove();
      } else {
        undoMove();
      }

      gameRunning = true;
      drawBoard();
    }
  }

  if (vsAI && !whiteTurn && gameRunning) {
    aiTurn();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_CANCEL, INPUT_PULLUP);

  randomSeed(analogRead(34));

  tft.begin();
  tft.setRotation(1);

  setupBoard();
  drawMenu();
}

void loop() {
  if (menuActive) {
    handleMenu();
  } else {
    handleGame();
  }

  delay(10);
}
