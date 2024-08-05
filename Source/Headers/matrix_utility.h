#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>


    

void printMatrix(const std::vector<std::vector<int>>& matrix);

std::vector<std::vector<int>> rotateMatrix(const std::vector<std::vector<int>>& matrix);
std::vector<std::vector<int>> removeBoardFromMatrix(const std::vector<std::vector<int>>& matrix);

std::vector<std::vector<int>> getBoardFromMatrix(const std::vector<std::vector<int>>& matrix, int border);
std::vector<std::vector<int>> getBoardFromMatrix( std::vector<std::vector<int>> matrix);

void addCathedral(std::vector<std::vector<int>>& pieceMap);


