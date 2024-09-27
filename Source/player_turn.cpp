#include <SFML/Graphics.hpp>

#include "Headers/player_turn.h"
#include "Headers/global.h"
#include "Headers/draw_board.h"
// #include "Headers/board_utility.h"
#include "Headers/matrix_utility.h"
#include "Headers/mcts.h"

#include <iostream>
#include <cmath>
#include <queue>

Cathedral_state player_turn::turn(sf::RenderWindow& window, sf::Event event, Cathedral_state* state, MCTS_tree *game_tree) {
    //function to add to state are //get a playerShape
    //check collision
    //check not opponent territory 
    //check if creating territory //these could be seperate class maybe?
    
    //using mcts move instead of of single piecemap etc 

    std::vector<std::vector<int>> singlePieceMap;
    vector<std::vector<std::vector<int>>> playersShapes; 
    
    _player = state->get_state_info().turn;

    if(_player == 1){
         playersShapes = state->get_state_info().player1Shapes;
        // _playerMin = player1Min;
        // _playerMax = player1Max; 

        
    } 
    else{
        playersShapes = state->get_state_info().player2Shapes;

        // _playerMin = player2Min;
        // _playerMax = player2Max; 
    }

    int pieceNum = 0;
  


    std::string inputString;
    bool selectingPiece = true;
    bool turnComplete = false;

    while(!turnComplete) {

        while(selectingPiece == true){
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return *state; // Exit early if the window is closed
                }
                
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128) { // Check for printable characters
                        char enteredChar = static_cast<char>(event.text.unicode);
                        if (enteredChar == '\b') { // Handle backspace
                            if (!inputString.empty()) {
                                inputString.pop_back();
                            }
                        } else if (enteredChar == '\r') { // Handle enter
                            try {
                                pieceNum = std::stoi(inputString); // Convert to integer
                                if(pieceNum >= playersShapes.size()){
                                    cout << "Input to large try again. (" << pieceNum << ") Valid size: " << playersShapes.size()-1 << endl;
                                }
                                else {
                                    std::cout << "Entered number: " << pieceNum << std::endl;
                                    selectingPiece = false;
                                }
                                
                            } catch (const std::invalid_argument&) {
                                std::cerr << "Invalid input. Not a number." << std::endl;
                            }
                            inputString.clear(); // Clear input after processing
                        } else if (isdigit(enteredChar)) { // Only accept digits
                            inputString += enteredChar;
                        }
                    }
                }
            }
        }

        singlePieceMap = playersShapes[pieceNum];

        bool validPlacement = false;
        while (!validPlacement) {
            // Update mouse position
            sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosWindow);

            sf::Sprite sprite;
            sf::Texture texture;
            texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

            sprite.setTexture(texture);
            window.clear();
            drawBackground(window);
            drawPieces(window, state);
            drawBoard(window, state);


            movePiecesTogether(window, sprite, mousePosWorld, singlePieceMap);
            window.display();


            while (window.pollEvent(event)) {
                // Check for close event to exit the game loop
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                    window.clear();
                    drawBackground(window);
                    drawBoard(window,state);
                    drawPieces(window, state);
                    window.display();

                    selectingPiece = true;
                    validPlacement = true; 
                    break;

                } 
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    state->change_turn();
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
                    static bool ignoreOnFirstTurn = false; //so we dont check if creating on first turn 
                    
                        vector<vector<int>> board = state->get_state_info().board;

                        auto [gridCol, gridRow] = convertMousePosToGridCoords(singlePieceMap, mousePosWorld);
                        Cathedral_move move(gridRow - minRow, gridCol - minCol, singlePieceMap);

                        bool noCollision = state->legal_move(&move); //also should checks territory 

                            if(noCollision){
                                game_tree->advance_tree(&move);
                                state->play_move(&move);
                                validPlacement = true;
                                turnComplete = true;
                            
                        
                        }
                    
                }
            }
        }
    }

    return *state;
}
    


//might be useless

// void player_turn::addShapeToMatrix(std::vector<std::vector<int>>& pieceMap, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
//     auto [gridX, gridY] = convertMousePosToGridCoords(shape, mousePosWorld);

//     // Iterate over the shape matrix and add it to the pieceMap
//     for (int i = 0; i < shape.size(); ++i) {
//         for (int j = 0; j < shape[0].size(); ++j) {
//             // Ensure the shape stays within the pieceMap boundaries
//             if (gridY + i < pieceMap.size() && gridX + j < pieceMap[0].size()) {
//                 // Only add non-zero values from the shape
//                 if (shape[i][j] >= _playerMin && shape[i][j] <= _playerMax) {
//                     pieceMap[gridY + i][gridX + j] = shape[i][j];
//                 }
//             }
//         }
//     }
// }



void player_turn::movePiecesTogether(sf::RenderWindow& window, sf::Sprite& sprite, sf::Vector2f mousePosWorld, std::vector<std::vector<int>>& singlePieceMap) {
    int firstSquareX = -1, firstSquareY = -1;

    // Find the first square in the shape
    for (int i = 0; i < singlePieceMap.size(); ++i) {
        for (int j = 0; j < singlePieceMap[i].size(); ++j) {
            if (singlePieceMap[i][j] != 0) {
                firstSquareX = j;
                firstSquareY = i;
                break;
            }
        }
        if (firstSquareX != -1 && firstSquareY != -1) {
            break;
        }
    }

    for (int i = 0; i < singlePieceMap.size(); ++i) {
        for (int j = 0; j < singlePieceMap[i].size(); ++j) {
            if (singlePieceMap[i][j] != 0) {
                float offsetX = (j - firstSquareX) * GRID_SIZE -8 ;
                float offsetY = (i - firstSquareY) * GRID_SIZE - 8;
                sprite.setPosition(mousePosWorld.x + offsetX, mousePosWorld.y + offsetY);
                sprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
                window.draw(sprite);
            }
        }
    }

    // window.display();
}






// Function to check if a cell is within the grid and is of players numder
// bool player_turn::isValid(const std::vector<std::vector<int>>& matrix, int x, int y) {
//     return x >= 0 && x < matrix.size() && y >= 0 && y < matrix[0].size() && matrix[x][y] >= _playerMin && matrix[x][y] <= _playerMax;
// }

// std::vector<std::vector<int>> player_turn::findShape(std::vector<std::vector<int>>& matrix, int startX, int startY) {
//     positions.clear();
//     if (!isValid(matrix, startX, startY)) {
//         return {}; 
//     }

//     int minCol = startX, maxCol = startX, minRow = startY, maxRow = startY;

//     std::queue<std::pair<int, int>> q;
//     q.push({startX, startY});

//     // To track visited cells
//     std::vector<std::vector<bool>> visited(matrix.size(), std::vector<bool>(matrix[0].size(), false));
//     visited[startX][startY] = true;

    

//     while (!q.empty()) {
//         std::pair<int, int> front = q.front();
//         int x = front.first;
//         int y = front.second;
//         q.pop();

//         positions.push_back({x, y});

//         // Update the bounds of the component
//         minCol = std::min(minCol, x);
//         maxCol = std::max(maxCol, x);
//         minRow = std::min(minRow, y);
//         maxRow = std::max(maxRow, y);

//         // Check all 4 directions
//         for (const auto& dir : directions) {
//             int newX = x + dir.first;
//             int newY = y + dir.second;

//             if (isValid(matrix, newX, newY) && !visited[newX][newY]) {
//                 visited[newX][newY] = true;
//                 q.push({newX, newY});
//             }
//         }
//     }
//     int pieceNum = matrix[startX][startY];
//     // Create the result matrix within the bounding rectangle and delete from old position 
//     std::vector<std::vector<int>> result(maxCol - minCol + 1, std::vector<int>(maxRow - minRow + 1, 0));
//     for (const auto& pos : positions) {
//         result[pos.first - minCol][pos.second - minRow] = pieceNum;
//         matrix[pos.first][pos.second] = 0;
//     }

//     return result;
// }




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
