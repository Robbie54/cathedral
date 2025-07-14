#pragma once

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"
#include "../../MonteCarloTreeSearch-main/mcts/include/mcts.h"
#include "Mcts.h"
#include <vector>



class PlayerTurn {

public:
    Cathedral_state turn(sf::RenderWindow& window, sf::Event event, Cathedral_state* state, MCTS_tree *game_tree); //should just pass the move back not handle game tree here

private:
    // Direction vectors for moving in the 4 cardinal directions (up, down, left, right)
    const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    int _player; 
    int _playerMin;
    int _playerMax;

    // To store the positions of the piece that is being moved incase of cancel 
    std::vector<std::pair<int, int>> positions;


    //ensuring mouse is over shape
    std::pair<int, int> convertMousePosToGridCoords(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld); 

    int getUserSelectedPiece(sf::RenderWindow& window, sf::Event& event, const std::vector<std::vector<std::vector<int>>>& shapes);
    void renderTurnPreview(sf::RenderWindow& window,Cathedral_state* state, const std::vector<std::vector<int>>& shape,const sf::Vector2f& mousePosWorld);
};



class PieceManipulator {
public:
    
    static std::vector<sf::Vector2f> getPolyomioPositions(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld);

private:

};




