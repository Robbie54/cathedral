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
constexpr unsigned int player1Max = 24;  //just adding lee way for pieces taken out etc 
constexpr unsigned int player1Territory = 25;
constexpr unsigned int player2Min = 26; 
constexpr unsigned int player2Max = 49; 
constexpr unsigned int player2Territory = 50;



const std::vector<std::vector<std::vector<int>>> shapeFullListPOne{  
    {{2}}, //Tavern
    {{3}},
    {{4, 4}}, //Stable
    {{5, 5}},
    {{6, 6},{0,6}}, //Inn
    {{7,7},{0,7}},
    {{8,8,8}}, //Bridge
    {{9, 9},{9,9}}, //Square
    {{10, 10,10},{0,10,0}}, //Manor
    {{0, 11,11},{11,11,0}}, //Abbey
    {{12, 12,0},{0,12,12}, {0,12,0}}, //Academy
    {{0, 13,0},{13,13,13}, {0,13,0}}, //Infirmary 
    {{14, 14,14},{14,0,14}}, //Castle
    {{0, 15,15},{15,15,0},{15,0,0}}}; //Tower

const std::vector<std::vector<std::vector<int>>> shapeFullListPTwo{  
    {{26}}, //Tavern
    {{27}}, //Tavern2
    {{28, 28}}, //Stable
    {{29, 29}}, //Stable2
    {{30, 30},{0,30}}, //Inn
    {{31,31},{0,31}}, //Inn2
    {{32,32,32}}, //Bridge
    {{33, 33},{33,33}}, //Square
    {{34, 34,34},{0,34,0}}, //Manor
    {{0, 35,35},{35,35,0}}, //Abbey
    {{36, 36,0},{0,36,36}, {0,36,0}}, //Academy
    {{0, 37,0},{37,37,37}, {0,37,0}}, //Infirmary 
    {{38, 38,38},{38,0,38}}, //Castle
    {{0, 39,39},{39,39,0},{39,0,0}}}; //Tower
