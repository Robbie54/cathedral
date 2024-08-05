#pragma once

#include <vector>

constexpr int GRID_SIZE = 16;
constexpr unsigned int SCREEN_HEIGHT = 16;
constexpr unsigned int SCREEN_WIDTH = 40;
constexpr unsigned int FONT_HEIGHT = 16;
constexpr unsigned int SCREEN_RESIZE = 2;
constexpr unsigned int BOARD_SIZE = 10;


//board dimensions
constexpr unsigned int minX = SCREEN_WIDTH / 2 - BOARD_SIZE / 2;
constexpr unsigned int maxX = SCREEN_WIDTH / 2 + BOARD_SIZE / 2 - 1;
constexpr unsigned int minY = SCREEN_HEIGHT / 2 - BOARD_SIZE / 2;
constexpr unsigned int maxY = SCREEN_HEIGHT / 2 + BOARD_SIZE / 2 - 1;


constexpr unsigned int empty = 0; 
constexpr unsigned int cathedral = 1; 
constexpr unsigned int player1Min = 2; 
constexpr unsigned int player1Max = 15; 
constexpr unsigned int player2Min = 16; 
constexpr unsigned int player2Max = 29; 


extern std::vector<std::vector<std::vector<int>>> shapeFullList;