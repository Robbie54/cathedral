
#include "Headers/Global.h"
#include "Headers/CathedralState.h"
#include <SFML/Graphics.hpp>
#include "Headers/MatrixUtility.h"

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

Cathedral_move generateRandomCathedralMove() {
    std::vector<std::vector<int>> shape = {{0, cathedral, 0}, {cathedral, cathedral, cathedral}, {0, cathedral, 0}, {0, cathedral, 0}};
    std::srand(static_cast<unsigned int>(std::time(0)));
    int rotations = std::rand() % 4;
    for (int i = 0; i < rotations; ++i) shape = rotateMatrix(shape);

    int shapeRows = shape.size(), shapeCols = shape[0].size();
    int maxStartRow = BOARD_SIZE - shapeRows, maxStartCol = BOARD_SIZE - shapeCols;
    int startRow = std::rand() % (maxStartRow + 1);
    int startCol = std::rand() % (maxStartCol + 1);

    return Cathedral_move(startRow, startCol, shape);
}



vector<vector<int>> updatePieceMapWithShapesRemaining(std::vector<std::vector<std::vector<int>>> shapeList, std::vector<std::vector<std::vector<int>>> shapeList2) {

    vector<vector<int>> shapeListFull; 

    int currentRow = 0, currentCol = 0;
    int maxHeightInRow = 0; // To keep track of the tallest shape in the current row
   
    vector<vector<vector<int>>> shapes = shapeList;

    vector<vector<int>> resultMatrix((PIECE_SPACE_WIDTH), vector<int>(PIECE_SPACE_HEIGHT,0));
    
    shapeListFull = updateIndividualPlayerPieceMap(shapeList);

    // Shift shapeListFull one column to the right
    for (auto& row : shapeListFull) {
        row.insert(row.begin(), 0);
    }
    
    for (int i = 0; i < shapeListFull.size(); ++i) {
        shapeListFull[i].resize(shapeListFull[i].size() + BOARD_SIZE-1, 0);
    }

    // Generate the matrix for shapeList2
    vector<vector<int>> shapeListFull2 = updateIndividualPlayerPieceMap(shapeList2);

    // Find the maximum number of rows between the two matrices
    int maxRows = std::max(shapeListFull.size(), shapeListFull2.size());

    // Resize both matrices to have the same number of rows
    shapeListFull.resize(maxRows, vector<int>(shapeListFull[0].size(), 0));
    shapeListFull2.resize(maxRows, vector<int>(shapeListFull2[0].size(), 0));

    // Concatenate shapeListFull2 to the right of shapeListFull
    for (int i = 0; i < maxRows; ++i) {
        shapeListFull[i].insert(shapeListFull[i].end(), shapeListFull2[i].begin(), shapeListFull2[i].end());
    }

    return shapeListFull;
   
}

vector<vector<int>> updateIndividualPlayerPieceMap(std::vector<std::vector<std::vector<int>>> shapeList) {

    int currentRow = 0, currentCol = 0;
    int maxHeightInRow = 0; // To keep track of the tallest shape in the current row
   
    vector<vector<int>> resultMatrix((PIECE_SPACE_WIDTH), vector<int>(PIECE_SPACE_HEIGHT,0));
    

    for (const auto& shape : shapeList) {
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        
        // If the shape does not fit horizontally, move to the next row
        if (currentCol + shapeWidth > PIECE_SPACE_WIDTH) {
            currentRow += maxHeightInRow + 1; // Move to the next row with a gap
            currentCol = 0; // Reset the column position
            maxHeightInRow = 0; // Reset the max height for the new row
        }

        // If the shape does not fit vertically, stop placing shapes
        if (currentRow + shapeHeight > PIECE_SPACE_HEIGHT) {
            cout << "Shapes cannot fit within the given dimensions." << endl;
            return resultMatrix;
        }

        // Place the shape in the result matrix
        for (int i = 0; i < shapeHeight; ++i) {
            for (int j = 0; j < shapeWidth; ++j) {
                resultMatrix[currentRow + i][currentCol + j] = shape[i][j];
            }
        }

        // Update current column and max height in the current row
        currentCol += shapeWidth + 1; // Move to the next position with a gap
        maxHeightInRow = max(maxHeightInRow, shapeHeight); // Update the max height if needed
    }

    return resultMatrix;
}


vector<vector<int>> extractShapeFromPosition(const vector<vector<int>>& pieceMap, int startX, int startY) {
    int rows = pieceMap.size();
    int cols = pieceMap[0].size();
    int targetIndex = pieceMap[startY][startX];
    if (targetIndex <= 0) return {}; // 0 or negative means empty or invalid

    // Find bounds of the shape
    int minX = startX, maxX = startX, minY = startY, maxY = startY;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    queue<pair<int, int>> q;
    q.push({startY, startX});
    visited[startY][startX] = true;

    // Directions: up, down, left, right
    const int dx[] = {0, 0, -1, 1};
    const int dy[] = {-1, 1, 0, 0};

    vector<pair<int, int>> shapeCells;

    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        shapeCells.push_back({y, x});
        minX = min(minX, x); maxX = max(maxX, x);
        minY = min(minY, y); maxY = max(maxY, y);

        for (int d = 0; d < 4; ++d) {
            int ny = y + dy[d], nx = x + dx[d];
            if (ny >= 0 && ny < rows && nx >= 0 && nx < cols &&
                !visited[ny][nx] && pieceMap[ny][nx] == targetIndex) {
                visited[ny][nx] = true;
                q.push({ny, nx});
            }
        }
    }

    // Build the shape matrix
    int shapeRows = maxY - minY + 1;
    int shapeCols = maxX - minX + 1;
    vector<vector<int>> shape(shapeRows, vector<int>(shapeCols, 0));
    for (const auto& [y, x] : shapeCells) {
        shape[y - minY][x - minX] = targetIndex;
    }
    return shape;
}