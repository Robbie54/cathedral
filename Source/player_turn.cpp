#include <SFML/Graphics.hpp>

#include "Headers/player_turn.h"
#include "Headers/global.h"
#include "Headers/draw_board.h"
#include "Headers/board_utility.h"
#include "Headers/matrix_utility.h"


#include <iostream>
#include <cmath>
#include <queue>

std::vector<std::vector<int>>& player_turn::turn(sf::RenderWindow& window, sf::Event event, std::vector<std::vector<int>>& pieceMap, int player, int turnCounter) {
    _player = player;
    if(player == 1){
        _playerMin = player1Min;
        _playerMax = player1Max; 

        
    } 
    else{
        _playerMin = player2Min;
        _playerMax = player2Max; 
    }
    
    bool turnComplete = false;

    while(!turnComplete) {
        bool decisionMade = false;
        std::vector<std::vector<int>> singlePieceMap;
        int pieceNum;

        while (!decisionMade) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return pieceMap;
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosWindow);

                    if (pieceMap[std::floor(mousePosWorld.y / 16)][std::floor(mousePosWorld.x / 16)] >= _playerMin && 
                        pieceMap[std::floor(mousePosWorld.y / 16)][std::floor(mousePosWorld.x / 16)] <= _playerMax) {
                        // Find the shape based on the piece selected
                        pieceNum = pieceMap[std::floor(mousePosWorld.y / 16)][std::floor(mousePosWorld.x / 16)];
                        singlePieceMap = findShape(pieceMap, std::floor(mousePosWorld.y / 16), std::floor(mousePosWorld.x / 16));
                        
                        decisionMade = true;
                        break;
                    }
                }
            }

        }

        bool validPlacement = false;
        while (!validPlacement) {
            // Update mouse position
            sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosWindow);

            sf::Sprite sprite;
            sf::Texture texture;
            texture.loadFromFile("/home/robbie/Desktop/Capstone/cathedral/Source/Images/misc.png");

            sprite.setTexture(texture);
            window.clear();
            drawBoard(window);
            drawPieces(window, pieceMap);
            movePiecesTogether(window, sprite, mousePosWorld, singlePieceMap);
            window.display();

            
            while (window.pollEvent(event)) {
                // Check for close event to exit the game loop
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return pieceMap;
                }

                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                    for (const auto& pos : positions) {
                        pieceMap[pos.first][pos.second] = pieceNum;
                    }
                     window.clear();
                    drawBoard(window);
                    drawPieces(window, pieceMap);
                    window.display();

                    decisionMade = false;
                    validPlacement = true; 
                    break;

                    
                    
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    for (const auto& pos : positions) {
                        pieceMap[pos.first][pos.second] = pieceNum;
                    }
                    decisionMade = true;
                    validPlacement = true; 
                    turnComplete = true;
                    break;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    singlePieceMap = rotateMatrix(singlePieceMap); // Rotate shape
                    movePiecesTogether(window, sprite, mousePosWorld, singlePieceMap);

                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                    sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosWindow);

                    // Check if the placement is valid
                    bool noCollision = checkCollision(pieceMap, singlePieceMap, mousePosWorld);
                    if(noCollision){
                        board_utility board2(player, getBoardFromMatrix(pieceMap)); 
                        validPlacement = board2.checkNotOpponentsTeritory(getBoardFromMatrix(pieceMap), mousePosWorld);

                    // validPlacement = true;
                    if (validPlacement) {

                        addShapeToMatrix(pieceMap, singlePieceMap, mousePosWorld);
                        
                        if(turnCounter > 1){
                            board_utility board2(player, getBoardFromMatrix(pieceMap)); 
                            bool remove = board2.checkIfCreatingTerritory(pieceMap, singlePieceMap, mousePosWorld);
                        }
                       
                        turnComplete = true;
                    }
                    
                    }
                }
            }

            
        }
    }
    return pieceMap;
    
}





bool player_turn::checkCollision(std::vector<std::vector<int>>& pieceMap, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
    auto [gridX, gridY] = convertMousePosToGridCoords(shape, mousePosWorld);

    // Iterate over the shape matrix
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[0].size(); ++j) {
            // Ensure the shape stays within the pieceMap boundaries
            if (gridY + i < pieceMap.size() && gridX + j < pieceMap[0].size()) {
                if (shape[i][j] != 0) {
                    // Check for collision with existing pieces
                    if (pieceMap[gridY + i][gridX + j] > 0) {
                        std::cout << "COLLISION please try again :) " << std::endl;
                        return false;
                    }

                    // Check if the piece is out of the board boundaries
                    if (gridY + i < SCREEN_HEIGHT / 2 - BOARD_SIZE / 2 ||
                        gridY + i >= SCREEN_HEIGHT / 2 + BOARD_SIZE / 2 ||
                        gridX + j < SCREEN_WIDTH / 2 - BOARD_SIZE / 2 ||
                        gridX + j >= SCREEN_WIDTH / 2 + BOARD_SIZE / 2) {
                        std::cout << "Out of grid :( " << std::endl;
                        return false;
                    }
                }
            }
        }
    }
    return true;
}



void player_turn::addShapeToMatrix(std::vector<std::vector<int>>& pieceMap, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
    auto [gridX, gridY] = convertMousePosToGridCoords(shape, mousePosWorld);

    // Iterate over the shape matrix and add it to the pieceMap
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[0].size(); ++j) {
            // Ensure the shape stays within the pieceMap boundaries
            if (gridY + i < pieceMap.size() && gridX + j < pieceMap[0].size()) {
                // Only add non-zero values from the shape
                if (shape[i][j] >= _playerMin && shape[i][j] <= _playerMax) {
                    pieceMap[gridY + i][gridX + j] = shape[i][j];
                }
            }
        }
    }
}



void player_turn::movePiecesTogether(sf::RenderWindow& window, sf::Sprite& sprite, sf::Vector2f mousePosWorld, std::vector<std::vector<int>>& singlePieceMap) {
    int firstSquareX = -1, firstSquareY = -1;

    // Find the first square in the shape
    for (int i = 0; i < singlePieceMap.size(); ++i) {
        for (int j = 0; j < singlePieceMap[i].size(); ++j) {
            if (singlePieceMap[i][j] >= _playerMin && singlePieceMap[i][j] <= _playerMax) {
                firstSquareX = j;
                firstSquareY = i;
                break;
            }
        }
        if (firstSquareX != -1 && firstSquareY != -1) {
            break;
        }
    }

    // Clear the window and draw the pieces
    for (int i = 0; i < singlePieceMap.size(); ++i) {
        for (int j = 0; j < singlePieceMap[i].size(); ++j) {
            if (singlePieceMap[i][j] >= _playerMin && singlePieceMap[i][j] <= _playerMax) {
                float offsetX = (j - firstSquareX) * GRID_SIZE -8 ;
                float offsetY = (i - firstSquareY) * GRID_SIZE - 8;
                sprite.setPosition(mousePosWorld.x + offsetX, mousePosWorld.y + offsetY);
                sprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
                window.draw(sprite);
            }
        }
    }
    window.display();
}






// Function to check if a cell is within the grid and is of players numder
bool player_turn::isValid(const std::vector<std::vector<int>>& matrix, int x, int y) {
    return x >= 0 && x < matrix.size() && y >= 0 && y < matrix[0].size() && matrix[x][y] >= _playerMin && matrix[x][y] <= _playerMax;
}

std::vector<std::vector<int>> player_turn::findShape(std::vector<std::vector<int>>& matrix, int startX, int startY) {
    positions.clear();
    if (!isValid(matrix, startX, startY)) {
        return {}; 
    }

    int minX = startX, maxX = startX, minY = startY, maxY = startY;

    std::queue<std::pair<int, int>> q;
    q.push({startX, startY});

    // To track visited cells
    std::vector<std::vector<bool>> visited(matrix.size(), std::vector<bool>(matrix[0].size(), false));
    visited[startX][startY] = true;

    

    while (!q.empty()) {
        std::pair<int, int> front = q.front();
        int x = front.first;
        int y = front.second;
        q.pop();

        positions.push_back({x, y});

        // Update the bounds of the component
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);

        // Check all 4 directions
        for (const auto& dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;

            if (isValid(matrix, newX, newY) && !visited[newX][newY]) {
                visited[newX][newY] = true;
                q.push({newX, newY});
            }
        }
    }
    int pieceNum = matrix[startX][startY];
    // Create the result matrix within the bounding rectangle and delete from old position 
    std::vector<std::vector<int>> result(maxX - minX + 1, std::vector<int>(maxY - minY + 1, 0));
    for (const auto& pos : positions) {
        result[pos.first - minX][pos.second - minY] = pieceNum;
        matrix[pos.first][pos.second] = 0;
    }

    return result;
}




std::pair<int, int> player_turn::convertMousePosToGridCoords(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
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
