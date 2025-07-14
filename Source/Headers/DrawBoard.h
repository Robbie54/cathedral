#pragma once
#include "Mcts.h"

#include "Global.h"


void drawBackground(sf::RenderWindow& window);
void drawBoard(sf::RenderWindow& window, Cathedral_state* state, int posX = minRow, int posY = minCol);
void drawPieces(sf::RenderWindow& window, Cathedral_state* state);
void drawMove(sf::RenderWindow& window, Cathedral_move* move, int posX, int posY);


