#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>


#include "CathedralState.h"


void printMatrix(const std::vector<std::vector<int>>& matrix);

std::vector<std::vector<int>> rotateMatrix(const std::vector<std::vector<int>>& matrix);
std::vector<std::vector<int>> removeBoardFromMatrix(const std::vector<std::vector<int>>& matrix);

std::vector<std::vector<int>> getBoardFromMatrix(const std::vector<std::vector<int>>& matrix, int border);
std::vector<std::vector<int>> getBoardFromMatrix( std::vector<std::vector<int>> matrix);

Cathedral_move generateRandomCathedralMove();

vector<vector<int>> updatePieceMapWithShapesRemaining(std::vector<std::vector<std::vector<int>>> shapeList, std::vector<std::vector<std::vector<int>>> shapeList2);

vector<vector<int>> updateIndividualPlayerPieceMap(std::vector<std::vector<std::vector<int>>> shapeList);

vector<vector<int>> extractShapeFromPosition(const vector<vector<int>>& pieceMap, int startX, int startY);
