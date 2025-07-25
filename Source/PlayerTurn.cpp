#include <SFML/Graphics.hpp>

#include "Headers/PlayerTurn.h"
#include "Headers/Global.h"
#include "Headers/MatrixUtility.h"

#include <iostream>
#include <cmath>
#include <queue>

// Static member definitions
int PlayerTurn::_player = 1;
int PlayerTurn::_playerMin = player1Min;
int PlayerTurn::_playerMax = player1Max;

std::vector<sf::Vector2f> PieceManipulator::getPolyomioPositions(
    const std::vector<std::vector<int>>& shape,
    const sf::Vector2f& mousePosWorld)
{
    std::vector<sf::Vector2f> positions;
    int firstX = -1, firstY = -1;

    // Find first occupied square
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {
                firstX = j;
                firstY = i;
                goto foundFirst;
            }
        }
    }

foundFirst:

    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {
                float offsetX = (j - firstX) * GRID_SIZE - 8;
                float offsetY = (i - firstY) * GRID_SIZE - 8;
                positions.emplace_back(mousePosWorld.x + offsetX, mousePosWorld.y + offsetY);
            }
        }
    }

    return positions;
}

// Converts mouse pos to top left cell where the shape would be placed, accounting for first top left square
std::pair<int, int> PlayerTurn::convertMousePosToGridCoords(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
    int gridX = mousePosWorld.x / GRID_SIZE;
    int gridY = mousePosWorld.y / GRID_SIZE;

    // Calculate the relative offset within the shape
    int relativeX = -1, relativeY = -1;
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {
                relativeX = j;
                relativeY = i;
                break;
            }
        }
        if (relativeX != -1 && relativeY != -1) {
            break;
        }
    }

    // Adjust grid coordinates based on the relative position of the mouse within the shape
    gridX -= relativeX;
    gridY -= relativeY;

    return {gridX, gridY};
}

std::vector<std::vector<int>> PlayerTurn::extractShapeFromPosition(
    const std::vector<std::vector<int>>& pieceMap, 
    int gridX, int gridY) {
    
    return extractShapeFromPosition(pieceMap, gridX, gridY);
}
