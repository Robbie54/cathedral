#include "Headers/matrix_utility.h"
#include "Headers/global.h"

#include <SFML/Graphics.hpp>




std::vector<std::vector<int>> rotateMatrix(const std::vector<std::vector<int>>& matrix) {
    int n = matrix.size(); // Number of rows
    int m = matrix[0].size(); // Number of columns

    // Create a new matrix with swapped dimensions
    std::vector<std::vector<int>> rotated(m, std::vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            rotated[j][n - i - 1] = matrix[i][j];
        }
    }

    return rotated;
}



void printMatrix(const std::vector<std::vector<int>>& matrix){
        std::cout << "{\n";
        for (const auto& row : matrix) {
            std::cout << "    {";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << row[i];
                if (i < row.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "},\n";
        }
        std::cout << "};\n";
}

std::vector<std::vector<int>> removeBoardFromMatrix(const std::vector<std::vector<int>>& matrix){
    std::vector<std::vector<int>> matrixTemp = matrix;
    for(int i = 0; i < matrixTemp.size(); i++){
        for(int j = 0; j < matrixTemp[i].size(); j++){
            if((j >= minX && j < maxX+1 && i >= minY && i < maxY+1)){
                matrixTemp[i][j] = 9;
            } 
        }
    }
    return matrixTemp;
}




//no border
std::vector<std::vector<int>> getBoardFromMatrix( std::vector<std::vector<int>> matrix){
    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    for (int i = minY; i <= maxY; ++i) {
        for (int j = minX; j <= maxX; ++j) {
            board[i - minY][j - minX] = matrix[i][j]; 

        }
    }


        return board;
}

std::vector<std::vector<int>> getBoardFromMatrix(const std::vector<std::vector<int>>& matrix, int border){
    std::vector<std::vector<int>> board(BOARD_SIZE + border*2, std::vector<int>(BOARD_SIZE + border*2, 4)); // Initialize with 1 extra border around


    for (int i = minY; i <= maxY; ++i) {
        for (int j = minX; j <= maxX; ++j) {
            board[i - minY + border][j - minX + border] = matrix[i][j]; // Offset by 1 to accommodate the border

        }
    }
      
        std::cout << "{\n";

        for (const auto& row : board) {
            std::cout << "    {";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << row[i];
                if (i < row.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "},\n";
        }
        std::cout << "};\n";


        return board;
}



void addCathedral(std::vector<std::vector<int>>& matrix) {
    std::vector<std::vector<int>> shape = {{0, cathedral, 0}, {cathedral, cathedral, cathedral}, {0, cathedral, 0}, {0,cathedral,0}};
    // Seed the random number generator with current time
    std::srand(static_cast<unsigned int>(std::time(0)));

      // Randomly decide how many times to rotate (0 to 3)
    int rotations = std::rand() % 4;

    // Rotate the shape randomly
    for (int i = 0; i < rotations; ++i) {
        shape = rotateMatrix(shape);
    }

    int matrixRows = matrix.size();
    int matrixCols = matrix[0].size();
    int shapeRows = shape.size();
    int shapeCols = shape[0].size();

    // Define bounds based on if statement conditions
    int minY = matrixRows/ 2 - BOARD_SIZE/2;
    int maxY = matrixRows / 2 + BOARD_SIZE/2 -1;
    int minX = matrixCols / 2 - BOARD_SIZE/2;
    int maxX = matrixCols / 2 + BOARD_SIZE/2 -1;

    // Calculate maximum allowable starting positions
    int maxStartRow = maxY - shapeRows + 1;
    int maxStartCol = maxX - shapeCols + 1;

    // Generate random position within bounds
    int startRow, startCol;
    do {
        startRow = std::rand() % maxStartRow + minY;
        startCol = std::rand() % maxStartCol + minX;
    } while (startRow < minY || startRow + shapeRows > maxY || startCol < minX || startCol + shapeCols > maxX);

    // Overlay the shape onto the large matrix
    for (int i = 0; i < shapeRows; ++i) {
        for (int j = 0; j < shapeCols; ++j) {
            // Only add non-zero values from the shape
            if (shape[i][j] != 0) {
                matrix[startRow + i][startCol + j] = shape[i][j];
            }
        }
    }
}



