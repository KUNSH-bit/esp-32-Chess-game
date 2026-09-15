# ESP32 Chess Game
A button-controlled chess game built for the ESP32. The project supports two-player chess as well as playing against a basic multi-level AI.

*Real-world hardware compatibility has not been verified.

## Features

* ESP32-based chess game
* ILI9341 TFT display
* Six-button control system
* Two-player mode
* Player vs AI mode
* Five AI difficulty levels
* Legal move validation
* Check detection
* Checkmate detection
* Stalemate detection
* Castling
* En passant
* Pawn promotion
* Promotion to Queen, Rook, Bishop, or Knight
* Move history
* Undo functionality
* Captured-piece handling
* Legal-move indicators
* Board coordinates
* Check indicator
* Game restart

## Hardware
The project is designed around:

* ESP32 DevKit
* ILI9341 TFT display
* 6 push buttons

### Button Connections

| Button | ESP32 GPIO |
| ------ | ---------: |
| Up     |    GPIO 32 |
| Down   |    GPIO 33 |
| Left   |    GPIO 25 |
| Right  |    GPIO 26 |
| Select |    GPIO 27 |
| Cancel |    GPIO 14 |

### ILI9341 Connections

| ILI9341 |                  ESP32 |
| ------- | ---------------------: |
| VCC     |                   3.3V |
| GND     |                    GND |
| CS      |                 GPIO 5 |
| D/C     |                 GPIO 2 |
| MOSI    |                GPIO 23 |
| SCK     |                GPIO 18 |
| MISO    |          Not connected |
| RST     | Not connected in Wokwi |
| LED     |                   3.3V |

The display configuration follows the SPI interface used by the ESP32.

## Controls

### Main Menu

* UP/DOWN: Select menu option
* SELECT: Confirm

### During Chess

* UP: Move cursor up
* DOWN: Move cursor down
* LEFT: Move cursor left
* RIGHT: Move cursor right
* SELECT: Select a piece or destination
* CANCEL: Cancel selection or undo the previous move

### Movement

Select a piece with the SELECT button, move the cursor to a legal destination, and press SELECT again.

Legal destinations are displayed on the board.

## Game Modes

### Two Player

Two players can play against each other using the same ESP32 and button controls.

### Player vs AI

The player controls White while the ESP32 controls Black.

Five AI levels are available:

| Level | Description                            |
| ----- | -------------------------------------- |
| 1     | Random legal move selection            |
| 2     | Basic material-based decision making   |
| 3     | Minimax-based search                   |
| 4     | Deeper minimax with alpha-beta pruning |
| 5     | Search with positional evaluation      |

The AI is designed as a lightweight chess opponent suitable for an ESP32 rather than as a replacement for a dedicated chess engine.

## Chess Rules Implemented

The game includes the following major chess rules:

* Pawn movement
* Pawn double-step
* Pawn captures
* Knight movement
* Bishop movement
* Rook movement
* Queen movement
* King movement
* Check
* Checkmate
* Stalemate
* Castling
* En passant
* Pawn promotion
* Illegal move prevention
* King safety validation

Promotion allows the player to choose between:

* Queen
* Rook
* Bishop
* Knight

## Software Requirements

The project uses:

* Arduino IDE
* ESP32 board support
* Adafruit GFX Library
* Adafruit ILI9341 Library

## Libraries

Install the following libraries through the Arduino IDE Library Manager:

```text
Adafruit GFX Library
Adafruit ILI9341
```

## Physical Hardware Status

The project has **not been tested on physical hardware**.

The code has been developed for the specified ESP32 and ILI9341 configuration and has been successfully compiled, but I currently do not have access to the required hardware to verify the physical implementation.

Therefore, the following should be considered unverified:

* Physical TFT display behavior
* Actual button response
* Electrical connections
* Display power requirements
* ESP32/TFT compatibility under real hardware conditions
* Long-term stability
* Performance of the AI on the physical ESP32

## Project Structure

```text
ESP32-Chess/
├── ESP32_Chess.ino
├── diagram.json
├── libraries.txt
└── README.md
```

## Limitations

This project is intended as an embedded-systems and programming project rather than a fully optimized chess engine.

Current limitations include:

* AI strength is limited by ESP32 processing resources
* No dedicated chess-engine opening database
* No online multiplayer
* No touchscreen interface
* No external storage
* No move notation system
* No physical hardware validation
* No dedicated graphical chess-piece sprites

## Future Improvements

Possible future improvements include:

* Custom graphical chess pieces
* More optimized AI
* Stronger chess evaluation
* Opening book
* Move notation
* Chess clock
* Save/load games
* External storage
* Battery-powered version
* Physical PCB
* Improved enclosure
* Larger display
* Real hardware testing

## Purpose

This project was created to explore the combination of:

* Embedded systems
* ESP32 programming
* TFT display interfacing
* Button-based user interfaces
* Chess algorithms
* Game-state management
* AI decision making
* Minimax algorithms
* Alpha-beta pruning
* Rule validation

It combines hardware-oriented programming with algorithmic problem solving in a single project.

## Status

**Current Status: Prototype**

* Source code: Complete
* Arduino compilation: Successful
* Physical hardware testing: Not performed
* Real-world hardware reliability: Not verified

## License
This project is available for educational and personal use. You may modify and extend the project for your own learning and experimentation.
