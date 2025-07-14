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
  


    bool turnComplete = false;

    while(!turnComplete) {
        int pieceNum = getUserSelectedPiece(window, event, playersShapes);
        if (pieceNum == -1) return *state; // User closed window


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

            renderTurnPreview(window, state, singlePieceMap, mousePosWorld);



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
                    auto posList = PieceManipulator::getPolyomioPositions(singlePieceMap, mousePosWorld);
                    for (const auto& pos : posList) {
                        sprite.setPosition(pos);
                        sprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
                        window.draw(sprite);
                    }
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


int player_turn::getUserSelectedPiece(sf::RenderWindow& window, sf::Event& event,
                                       const std::vector<std::vector<std::vector<int>>>& shapes)
{
    std::string inputString;
    while (true) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1; // signal cancel
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == '\b') {
                        if (!inputString.empty()) inputString.pop_back();
                    } else if (c == '\r') {
                        try {
                            int selected = std::stoi(inputString);
                            if (selected >= 0 && selected < shapes.size()) {
                                return selected;
                            } else {
                                std::cout << "Invalid input. Try 0-" << shapes.size() - 1 << std::endl;
                            }
                        } catch (...) {
                            std::cerr << "Invalid input.\n";
                        }
                        inputString.clear();
                    } else if (isdigit(c)) {
                        inputString += c;
                    }
                }
            }
        }
    }
}


void player_turn::renderTurnPreview(
    sf::RenderWindow& window,
    Cathedral_state* state,
    const std::vector<std::vector<int>>& shape,
    const sf::Vector2f& mousePosWorld)
{
    sf::Sprite sprite;
    sf::Texture texture;
    texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");
    sprite.setTexture(texture);

    window.clear();
    drawBackground(window);
    drawPieces(window, state);
    drawBoard(window, state);

    auto posList = PieceManipulator::getPolyomioPositions(shape, mousePosWorld);
    for (const auto& pos : posList) {
        sprite.setPosition(pos);
        sprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
        window.draw(sprite);
    }

    window.display();
}

