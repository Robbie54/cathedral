#pragma once
#include "CathedralState.h"

#include "Global.h"

class ScreenRenderer {
public:
    static void drawBackground(sf::RenderWindow& window);
    static void drawBoard(sf::RenderWindow& window, vector<vector<int>> board, int posX = minRow, int posY = minCol);
    static void drawUnplayedPieces(sf::RenderWindow& window, vector<vector<int>> pieceMap);
    static void drawMove(sf::RenderWindow& window, Cathedral_move* move, int posX, int posY);

};

