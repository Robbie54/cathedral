#pragma once

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"
#include "mcts.h"


class player_turn {

public:
    Cathedral_state turn(sf::RenderWindow& window, sf::Event event, Cathedral_state* state);
    std::vector<std::vector<int>> findShape( std::vector<std::vector<int>>& matrix, int startX, int startY);

private:
    // Direction vectors for moving in the 4 cardinal directions (up, down, left, right)
    const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    int _player; 
    int _playerMin;
    int _playerMax;

    // To store the positions of the piece that is being moved incase of cancel 
    std::vector<std::pair<int, int>> positions;


    //converted to class late thats why function variables are jank 

    void movePiecesTogether(sf::RenderWindow& window, sf::Sprite& sprite, sf::Vector2f mousePosWorld, std::vector<std::vector<int>>& singlePieceMap);

    bool isValid(const std::vector<std::vector<int>>& matrix, int x, int y);


    void addShapeToMatrix(std::vector<std::vector<int>>& pieceMap, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld);

    bool checkCollision(std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld);

    //ensuring mouse is over shape
    std::pair<int, int> convertMousePosToGridCoords(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld); 


};


