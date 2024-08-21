#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp> 

#include <vector>
#include <iostream>
#include <utility>
#include <unordered_set>
#include <boost/functional/hash.hpp>


#include "Headers/global.h"
#include "Headers/board_utility.h"
#include "Headers/matrix_utility.h"
#include "Headers/player_turn.h"


using namespace std;


bool board_utility::checkIfCreatingTerritory(std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
    bool valid;

    vector<pair<int,int>> positionsToCheck = positionsAroundShape(shape); //not sure which is X or Y 
   
    int gridCol = (mousePosWorld.x / GRID_SIZE) - minCol;
    int gridRow = (mousePosWorld.y / GRID_SIZE) - minRow;
    cout << "Checking at " << gridCol << " " << gridRow << endl;
    for (auto pos : positionsToCheck){
        int boardCol = gridCol + pos.second; //X
        int boardRow = gridRow + pos.first; //Y
        
        if(boardCol < 0 || boardCol >= _board[0].size() || boardRow < 0 || boardRow >= _board.size()){
            continue; 
        }
        if(_board[boardCol][boardRow] >= _playerMin && _board[boardCol][boardRow] <= _playerMax){
            continue; 
        }

        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        int pieceInside = INT_MAX; //max when no piece inside       
        piecePosToRemove.clear();
        pieceNum = 0;
        bool r = checkPositionForPieceRemoval(boardCol, boardRow, visited, pieceInside); 
        if(r == true){ 
            if(piecePosToRemove.empty()){
                cout << "EMPTY " << endl;
                return false;
            }
            pieceNum = board[piecePosToRemove[0].first][piecePosToRemove[0].second];
            for(auto pos : piecePosToRemove ){
                cout << "piece to remove " <<  pos.first << " " << pos.second << endl;
                board[pos.first][pos.second] = 0; 
            }
            // addShapeBack(map);
            cout << "not adding back " << endl;
        }

    }


   
    return false;
}

void board_utility::addShapeBack(std::vector<std::vector<int>>& map){
    if(pieceNum == 0 ){
        cout << "pieceNum is 0";
        return;
    }
    cout << "pieceNum is " << pieceNum << endl;
    for(int y = 0; y < map.size(); y++){
        for(int x = 0; x < map[0].size(); x++){
            if(startingMap[y][x] == pieceNum){
                map[y][x] = pieceNum;
            }
        }
    }
}



std::vector<std::pair<int, int>> board_utility::positionsAroundShape(const std::vector<std::vector<int>>& shape) {
    std::vector<std::pair<int, int>> positions;
    // Find the topmost and leftmost square in the shape matrix
    int topmostX = -1, topmostY = -1;
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {
                topmostX = j;
                topmostY = i;
                break;
            }
        }
        if (topmostX != -1 && topmostY != -1) {
            break;
        }
    }

    if (topmostX == -1 || topmostY == -1) {
        // Shape matrix is empty or invalid
        return positions;
    }

    // Compute positions around the shape
   for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j] != 0) {

                if (i == 0 || shape[i - 1][j] == 0) {
                    positions.push_back({i - topmostY - 1, j - topmostX});
                }
                if (i == shape.size() - 1 || shape[i + 1][j] == 0) {
                    positions.push_back({i - topmostY + 1, j - topmostX});
                }
                if (j == 0 || shape[i][j - 1] == 0) {
                    positions.push_back({i - topmostY, j - topmostX - 1});
                }
                if (j == shape[i].size() - 1 || shape[i][j + 1] == 0) {
                    positions.push_back({i - topmostY, j - topmostX + 1});
                }
                

            }
        }
    }

    return positions;
}


bool board_utility::checkPositionForPieceRemoval(int x, int y, std::vector<std::vector<bool>>& visited, int& pieceInside) {
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

    if (_board[y][x] >= _playerMin && _board[y][x] <= _playerMax) {
        return true;
    }


    if ((_board[y][x] >= _opponentMin && _board[y][x] <= _opponentMax) || _board[y][x] == cathedral) {
            
        if (pieceInside == INT_MAX || _board[y][x] == pieceInside ) {
            pieceInside = _board[y][x];
            piecePosToRemove.push_back(std::make_pair(y, x));
            cout << "pushing back y(row)  " << y << " x(col) " << x << endl;
        }
        else {
            cout <<"returning false " << piecePosToRemove.size() << " pieceInside " << pieceInside << " _board[y][x] " << _board[y][x] << endl;
            return false; 
        }

       
    }
   


    if (_board[y][x] == 0 || _board[y][x] == cathedral || (_board[y][x] >= _opponentMin && _board[y][x] <= _opponentMax)) {
        

        // Check all adjacent cells (including diagonals)
        bool valid = true;
        valid &= checkPositionForPieceRemoval(x + 1, y, visited, pieceInside); // Right
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x - 1, y, visited, pieceInside); // Left
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x, y + 1, visited, pieceInside); // Down
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x, y - 1, visited, pieceInside); // Up
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x + 1, y + 1, visited, pieceInside); // Bottom-Right (Diagonal)
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x - 1, y - 1, visited, pieceInside); // Top-Left (Diagonal)
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x + 1, y - 1, visited, pieceInside); // Top-Right (Diagonal)
        if (!valid) return false;
        valid &= checkPositionForPieceRemoval(x - 1, y + 1, visited, pieceInside); // Bottom-Left (Diagonal)
        if (!valid) return false;


        return valid;
    }

    return false;
}







bool board_utility::checkNotOpponentsTeritory(std::vector<std::vector<int>> board, const sf::Vector2f& mousePosWorld) {
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



bool board_utility::checkPosition(int x, int y, std::vector<std::vector<bool>>& visited) {
    // Check bounds
    if (y < 0 || y >= rows || x < 0 || x >= cols) {
        return true; // Reached the edge, valid
    }
    //this is like if you found yourself or the cathedral you know your not in someone elses territory 
    if (_board[y][x] >= _playerMin && _board[y][x] <= _playerMax || _board[y][x] == cathedral) {
        // cout << "At " << x << ", " << y << ", board value: " << _board[y][x] << " - false" << endl;
        return false;
    }

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