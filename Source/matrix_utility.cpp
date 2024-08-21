
#include "Headers/global.h"
#include "Headers/mcts.h"
#include <SFML/Graphics.hpp>
#include "Headers/matrix_utility.h"



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
            if((j >= minCol && j < maxCol+1 && i >= minRow && i < maxRow+1)){
                matrixTemp[i][j] = 9;
            } 
        }
    }
    return matrixTemp;
}




//no border
std::vector<std::vector<int>> getBoardFromMatrix( std::vector<std::vector<int>> matrix){
    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    for (int i = minRow; i <= maxRow; ++i) {
        for (int j = minCol; j <= maxCol; ++j) {
            board[i - minRow][j - minCol] = matrix[i][j]; 

        }
    }


        return board;
}

std::vector<std::vector<int>> getBoardFromMatrix(const std::vector<std::vector<int>>& matrix, int border){
    std::vector<std::vector<int>> board(BOARD_SIZE + border*2, std::vector<int>(BOARD_SIZE + border*2, 4)); // Initialize with 1 extra border around


    for (int i = minRow; i <= maxRow; ++i) {
        for (int j = minCol; j <= maxCol; ++j) {
            board[i - minRow + border][j - minCol + border] = matrix[i][j]; // Offset by 1 to accommodate the border

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


/**
 * todo check can place 
 */
Cathedral_move addCathedral(std::vector<std::vector<int>>& matrix) {
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
    int minRow = matrixRows/ 2 - BOARD_SIZE/2;
    int maxRow = matrixRows / 2 + BOARD_SIZE/2 -1;
    int minCol = matrixCols / 2 - BOARD_SIZE/2;
    int maxCol = matrixCols / 2 + BOARD_SIZE/2 -1;

    // Calculate maximum allowable starting positions
    int maxStartRow = maxRow - shapeRows + 1;
    int maxStartCol = maxCol - shapeCols + 1;

    // Generate random position within bounds
    int startRow, startCol;
    do {
        startRow = std::rand() % maxStartRow + minRow;
        startCol = std::rand() % maxStartCol + minCol;
    } while (startRow < minRow || startRow + shapeRows > maxRow || startCol < minCol || startCol + shapeCols > maxCol);


    Cathedral_move m(startRow, startCol, shape); 
    return m;
}



//

