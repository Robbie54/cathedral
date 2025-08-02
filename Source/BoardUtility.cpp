#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp> 

#include <vector>
#include <iostream>
#include <utility>
#include <unordered_set>
#include <boost/functional/hash.hpp>


#include "Headers/Global.h"
#include "Headers/BoardUtility.h"
#include "Headers/MatrixUtility.h"
#include "Headers/PlayerTurn.h"
#include "Headers/ScreenRenderer.h"

#include "Headers/CathedralState.h"



using namespace std;



std::vector<std::pair<int, int>> BoardUtility::positionsAroundShape(const Cathedral_move* move) {
    std::vector<std::pair<int, int>> positions;
    const auto& shape = move->shape;
    int baseRow = move->row;
    int baseCol = move->col;

    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {
                int boardRow = baseRow + i;
                int boardCol = baseCol + j;

                // Up
                if (i == 0 || shape[i - 1][j] == 0) {
                    positions.push_back({boardRow - 1, boardCol});
                }
                // Down
                if (i == shape.size() - 1 || shape[i + 1][j] == 0) {
                    positions.push_back({boardRow + 1, boardCol});
                }
                // Left
                if (j == 0 || shape[i][j - 1] == 0) {
                    positions.push_back({boardRow, boardCol - 1});
                }
                // Right
                if (j == shape[i].size() - 1 || shape[i][j + 1] == 0) {
                    positions.push_back({boardRow, boardCol + 1});
                }
            }
        }
    }

    // Remove duplicates
    std::unordered_set<std::pair<int, int>, boost::hash<std::pair<int, int>>> uniquePositions(positions.begin(), positions.end());
    positions.assign(uniquePositions.begin(), uniquePositions.end());
    for (const auto& pos : positions) {
        std::cout << "Position: (" << pos.first << ", " << pos.second << ")" << std::endl;
    }
    return positions;
}

//unused? 
bool BoardUtility::checkPositionForPieceRemoval(int x, int y, std::vector<std::vector<bool>>& visited, int& pieceInside) {
    // Check bounds
    if (y < 0 || y >= rows || x < 0 || x >= cols) {
        return true; // Reached the edge, valid
    }

    // If already visited, skip this cell
    if (visited[y][x]) {
        return true;
    }

    // Mark the cell as visited
    visited[y][x] = true;

    int cellValue = _board[y][x];

    // If it's a player piece, it's valid
    if (cellValue >= _playerMin && cellValue <= _playerMax) {
        return true;
    }

    // If it's an opponent piece or cathedral, track it
    if ((cellValue >= _opponentMin && cellValue <= _opponentMax) || cellValue == cathedral) {
        if (pieceInside == INT_MAX || cellValue == pieceInside) {
            pieceInside = cellValue;
            piecePosToRemove.push_back({y, x});
        } else {
            return false; // Multiple different piece types
        }
    }

    // Continue flood fill for empty spaces or opponent territory
    if (cellValue == 0 || cellValue == _opponentTerritory) {
        bool valid = true;
        valid &= checkPositionForPieceRemoval(x + 1, y, visited, pieceInside); // Right
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x - 1, y, visited, pieceInside); // Left
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x, y + 1, visited, pieceInside); // Down
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x, y - 1, visited, pieceInside); // Up
        return valid;
    }

    return false;
}



//unused ? 
bool BoardUtility::checkNotOpponentsTerritory(std::vector<std::vector<int>> board, const sf::Vector2f& mousePosWorld) {
    bool valid = true;
    _board = board; 

    int gridX = mousePosWorld.x / GRID_SIZE - minCol; 
    int gridY = mousePosWorld.y / GRID_SIZE - minRow;
    
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));     
    valid = checkPosition(gridX, gridY, visited);
    if (!valid) {
        return true; 
    } else {
        std::cout << "In enemy's territory: at " << gridX << " " << gridY<< endl;
        
        return false;
    }

   return valid;
}


//unsused?
bool BoardUtility::checkPosition(int x, int y, std::vector<std::vector<bool>>& visited) {
    // Check bounds
    if (y < 0 || y >= rows || x < 0 || x >= cols) {
        return true; // Reached the edge, valid
    }
    //if you found yourself or the cathedral you know your not in someone elses territory 
    if (_board[y][x] >= _playerMin && _board[y][x] <= _playerMax || _board[y][x] == cathedral) {
        // cout << "At " << x << ", " << y << ", board value: " << _board[y][x] << " - false" << endl;
        return false;
    }
    //If opponent piece
    if (_board[y][x] >= _opponentMin && _board[y][x] <= _opponentMax) {
        return true;
    }

    if (_board[y][x] == 0 && !visited[y][x]) {
        // Mark the cell as visited
        visited[y][x] = true;

        // Check all adjacent cells (including diagonals)
        bool valid = true;
        valid &= checkPosition(x + 1, y, visited);     // Right
        if (!valid) return false;
        valid &= checkPosition(x - 1, y, visited);     // Left
        if (!valid) return false;
        valid &= checkPosition(x, y + 1, visited);     // Down
        if (!valid) return false;
        valid &= checkPosition(x, y - 1, visited);     // Up
        if (!valid) return false;
        valid &= checkPosition(x + 1, y + 1, visited); // Bottom-Right (Diagonal)
        if (!valid) return false;
        valid &= checkPosition(x - 1, y - 1, visited); // Top-Left (Diagonal)
        if (!valid) return false;
        valid &= checkPosition(x + 1, y - 1, visited); // Top-Right (Diagonal)
        if (!valid) return false;
        valid &= checkPosition(x - 1, y + 1, visited); // Bottom-Left (Diagonal)


        return valid;
    }

    return true;
}

// Territory management methods moved from CathedralState
void BoardUtility::setTerritoryInfo(int playerTerritory, int opponentTerritory) {
    _playerTerritory = playerTerritory;
    _opponentTerritory = opponentTerritory;
    pieceNumToRemove = -1;
}

bool BoardUtility::checkIfCreatingTerritory(const Cathedral_move *move) {
    vector<pair<int,int>> boardPositionsToCheck = positionsAroundShape(move);

    // Clear previous flood fills if in step-by-step mode
    if (stepByStepMode) {
        allFloodFills.clear();
        currentFloodFillIndex = 0;
    }

    for (auto pos : boardPositionsToCheck){
        int boardRow = pos.first; 
        int boardCol = pos.second; 

        // Check inside board bounds
        if(boardCol < 0 || boardCol >= _board[0].size() || boardRow < 0 || boardRow >= _board.size()){
            continue; 
        }

        //check if player piece
        if(_board[boardRow][boardCol] >= _playerMin && _board[boardRow][boardCol] <= _playerMax){
            continue; 
        }

        std::vector<std::vector<bool>> visited(_board.size(), std::vector<bool>(_board[0].size(), false));
        pieceNumToRemove = -1;
        int enclosedPieceCount = 0;
        
        // Clear visited positions for debug visualization
        if (debugMode) {
            visitedPositions.clear();
        }
        
        bool isTerritory = checkIfPositionIsNowPlayersTerritory(boardRow, boardCol, visited, enclosedPieceCount); 

        // Store this flood fill if in step-by-step mode
        if (stepByStepMode && debugMode && !visitedPositions.empty()) {
            allFloodFills.push_back(visitedPositions);
            std::cout << "Captured flood fill " << allFloodFills.size() << " starting at (" 
                      << boardRow << ", " << boardCol << ") with " << visitedPositions.size() << " positions" << std::endl;
        }

        if(isTerritory && enclosedPieceCount <= 1){ 
            changeSpaceToPlayersTerritory(boardRow, boardCol);
            return true;
        }
    }

    if (stepByStepMode && !allFloodFills.empty()) {
        std::cout << "Territory analysis complete. Captured " << allFloodFills.size() << " flood fills. Press N to step through them." << std::endl;
    }

    return false;
}

bool BoardUtility::checkIfPositionIsNowPlayersTerritory(int row, int col, std::vector<std::vector<bool>>& visited, int& enclosedPieceCount){

    // Check bounds - board edges count as valid enclosure
    if (row < 0 || row >= _board.size() || col < 0 || col >= _board[0].size()) { 
        return true;
    }

    // If already visited, skip this cell
    if (visited[row][col]) { 
        return true;
    }

    // Mark the cell as visited
    visited[row][col] = true;
    
    // Add to debug visualization if enabled
    if (debugMode) {
        visitedPositions.push_back({row, col});
        // Removed excessive debug output for cleaner console during gameplay
    }
    
    int cellValue = _board[row][col];

    // If it's player's own piece or territory, this forms valid enclosure
    if (cellValue >= _playerMin && cellValue <= _playerMax || cellValue == _playerTerritory) {
        return true;
    }

    // If it's an opponent piece or cathedral that can be captured
    if ((cellValue >= _opponentMin && cellValue <= _opponentMax) || cellValue == cathedral) {  

        enclosedPieceCount++;
        
        // Track pieces that need to be removed - only single pieces can be enclosed
        if(pieceNumToRemove == -1){
           pieceNumToRemove = cellValue;
        } else if(cellValue != pieceNumToRemove) {
            return false; // Multiple different piece types
        }
    }

    // For empty spaces, opponent territory, or pieces marked for removal, continue flood fill
    if (cellValue == 0 || cellValue == cathedral || 
        (cellValue >= _opponentMin && cellValue <= _opponentMax) || 
        cellValue == _opponentTerritory) { 
        
        // Check all 8 adjacent cells (including diagonals)
        bool valid = true;
        valid &= checkIfPositionIsNowPlayersTerritory(row, col + 1, visited, enclosedPieceCount); // Right
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row, col - 1, visited, enclosedPieceCount); // Left
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row + 1, col, visited, enclosedPieceCount); // Down
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row - 1, col, visited, enclosedPieceCount); // Up
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row + 1, col + 1, visited, enclosedPieceCount); // Bottom-Right
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row - 1, col - 1, visited, enclosedPieceCount); // Top-Left
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row + 1, col - 1, visited, enclosedPieceCount); // Bottom-Left
        if (!valid) return false;
        valid &= checkIfPositionIsNowPlayersTerritory(row - 1, col + 1, visited, enclosedPieceCount); // Top-Right
        if (!valid) return false;

        return valid;
    }

    return false;
}

void BoardUtility::changeSpaceToPlayersTerritory(int row, int col){
    // Boundary check
    if (row < 0 || col < 0 || row >= _board.size() || col >= _board[0].size()) { 
        return;
    }
    
    int cellValue = _board[row][col];

    if (cellValue != 0 && cellValue != pieceNumToRemove && cellValue != _opponentTerritory) { 
        return;
    }
    
    // Mark as player territory
    _board[row][col] = _playerTerritory;
    
    // Recursively mark adjacent orthogonal positions only (using the 4-direction approach)
    const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (const auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        changeSpaceToPlayersTerritory(newRow, newCol);
    }
}

void BoardUtility::drawVisitedPositions() {
    if (!debugMode || !debugWindow) {
        return;
    }
    
    if (stepByStepMode && currentFloodFillIndex < allFloodFills.size()) {
        // Draw only the current flood fill
        const auto& currentFloodFill = allFloodFills[currentFloodFillIndex];
        // std::cout << "Drawing flood fill " << (currentFloodFillIndex + 1) << "/" << allFloodFills.size() 
        //           << " with " << currentFloodFill.size() << " positions" << std::endl;
        
        for (const auto& pos : currentFloodFill) {
            int gridX = pos.second + minCol;
            int gridY = pos.first + minRow;
            ScreenRenderer::drawSquareClicked(*debugWindow, gridX, gridY);
        }
    } else {
        // Draw all visited positions (original behavior)
        std::cout << "Debug: Drawing " << visitedPositions.size() << " visited positions" << std::endl;
        
        for (const auto& pos : visitedPositions) {
            int gridX = pos.second + minCol;
            int gridY = pos.first + minRow;
            ScreenRenderer::drawSquareClicked(*debugWindow, gridX, gridY);
        }
    }
}

void BoardUtility::nextFloodFill() {
    if (stepByStepMode && currentFloodFillIndex < allFloodFills.size() - 1) {
        currentFloodFillIndex++;
        std::cout << "Advanced to flood fill " << (currentFloodFillIndex + 1) << "/" << allFloodFills.size() << std::endl;
    } else if (stepByStepMode) {
        std::cout << "Already at the last flood fill (" << allFloodFills.size() << " total)" << std::endl;
    }
}

/*
 * USAGE EXAMPLE FOR TERRITORY DEBUGGING VISUALIZATION:
 * 
 * To enable visualization of positions visited during territory checking:
 * 
 * 1. In GameManager.cpp (or wherever you handle moves), before calling territory checking:
 * 
 *    // Create BoardUtility instance
 *    BoardUtility boardUtil(currentPlayer, board);
 *    boardUtil.setTerritoryInfo(playerTerritory, opponentTerritory);
 *    
 *    // Enable debug visualization (pass the render window)
 *    boardUtil.enableDebugVisualization(&window);
 *    
 *    // Check for territory creation
 *    bool territoryCreated = boardUtil.checkIfCreatingTerritory(move);
 *    
 *    // In your render loop, after drawing the board:
 *    ScreenRenderer::drawBoard(window, board);
 *    boardUtil.drawVisitedPositions();  // This will highlight visited squares
 *    
 *    // Don't forget to disable when done
 *    boardUtil.disableDebugVisualization();
 * 
 * 2. Alternative usage with manual control (e.g., press 'D' to toggle debug mode):
 * 
 *    // In your event handling:
 *    if (event.key.code == sf::Keyboard::D) {
 *        debugTerritoryMode = !debugTerritoryMode;
 *        if (debugTerritoryMode) {
 *            std::cout << "Territory debug visualization enabled" << std::endl;
 *        } else {
 *            std::cout << "Territory debug visualization disabled" << std::endl;
 *        }
 *    }
 *    
 *    // When creating BoardUtility:
 *    BoardUtility boardUtil(currentPlayer, board);
 *    if (debugTerritoryMode) {
 *        boardUtil.enableDebugVisualization(&window);
 *    }
 *    
 * The visualization will show red squares (from misc.png texture) on all positions
 * that the checkIfPositionIsNowPlayersTerritory() function visits during its
 * flood-fill algorithm, helping you debug territory detection logic.
 */