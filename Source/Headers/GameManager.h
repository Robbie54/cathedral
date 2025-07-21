#pragma once

#include <SFML/Graphics.hpp>
#include "CathedralState.h"
#include "PlayerTurn.h"

class GameManager {
public:
    GameManager(sf::RenderWindow& window);
    ~GameManager();

    void run();

private:
    void initialiseGame();
    bool processEvents();
    void performPlayerMove();
    void performMCTSMove();

    sf::RenderWindow& window;
    sf::Event event;
    const Cathedral_state* state;
    MCTS_tree* game_tree;
    int winner;
};
