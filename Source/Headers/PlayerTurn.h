#pragma once

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"
#include "../../MonteCarloTreeSearch-main/mcts/include/mcts.h"
#include <vector>

class PlayerTurn {
public:
    // Static utility methods for piece manipulation
    static std::pair<int, int> convertMousePosToGridCoords(
        const std::vector<std::vector<int>>& shape, 
        const sf::Vector2f& mousePosWorld);
    
    static std::vector<std::vector<int>> extractShapeFromPosition(
        const std::vector<std::vector<int>>& pieceMap, 
        int gridX, int gridY);

private:
    static int _player;
    static int _playerMin;
    static int _playerMax;
};

class PieceManipulator {
public:
    static std::vector<sf::Vector2f> getPolyomioPositions(
        const std::vector<std::vector<int>>& shape,
        const sf::Vector2f& mousePosWorld);
};




