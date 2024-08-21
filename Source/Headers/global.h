#pragma once

#include <vector>

constexpr int GRID_SIZE = 16;
constexpr unsigned int SCREEN_HEIGHT = 16;
constexpr unsigned int SCREEN_WIDTH = 40;
constexpr unsigned int FONT_HEIGHT = 16;
constexpr unsigned int SCREEN_RESIZE = 2;
constexpr unsigned int BOARD_SIZE = 10;

constexpr unsigned int PIECE_SPACE_HEIGHT = SCREEN_HEIGHT;
constexpr unsigned int PIECE_SPACE_WIDTH = SCREEN_WIDTH/2- BOARD_SIZE / 2 - 2;

//board dimensions
constexpr unsigned int minCol = SCREEN_WIDTH / 2 - BOARD_SIZE / 2;
constexpr unsigned int maxCol = SCREEN_WIDTH / 2 + BOARD_SIZE / 2 - 1;
constexpr unsigned int minRow = SCREEN_HEIGHT / 2 - BOARD_SIZE / 2;
constexpr unsigned int maxRow = SCREEN_HEIGHT / 2 + BOARD_SIZE / 2 - 1;


constexpr unsigned int empty = 0; 
constexpr unsigned int cathedral = 1; 
constexpr unsigned int player1Min = 2; 
constexpr unsigned int player1Max = 25;  //just adding lee way for pieces taken out etc 
constexpr unsigned int player2Min = 26; 
constexpr unsigned int player2Max = 50; 


extern std::vector<std::vector<std::vector<int>>> shapeFullList;