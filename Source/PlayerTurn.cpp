#include <SFML/Graphics.hpp>

#include "Headers/PlayerTurn.h"
#include "Headers/Global.h"
#include "Headers/ScreenRenderer.h"
// #include "Headers/BoardUtility.h"
#include "Headers/MatrixUtility.h"
#include "Headers/CathedralState.h"

#include <iostream>
#include <cmath>
#include <queue>


//from examples they have a state that gets changed in main loop and a seperate game_tree. It is necassary to ensure you update them and keep them the same
void PlayerTurn::turn(sf::RenderWindow& window, sf::Event event, MCTS_tree *game_tree) {

    const Cathedral_state* state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());    
    const vector<vector<int>> pieceMap = updatePieceMapWithShapesRemaining(state->get_state_info().player1Shapes, state->get_state_info().player2Shapes);


    std::vector<std::vector<int>> singlePieceMap;

    _player = state->get_state_info().turn;

    if(_player == 1){
        _playerMin = player1Min;
        _playerMax = player1Max; 

        
    } 
    else{
        _playerMin = player2Min;
        _playerMax = player2Max; 
    }

    int pieceNum = 0;

    bool turnComplete = false;

    while(!turnComplete) {
            std::vector<std::vector<int>> singlePieceMap = getUserSelectedPiece(window, event, pieceMap);
        if (pieceNum == -1) return; // User closed window

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
                    ScreenRenderer::drawBackground(window);
                    ScreenRenderer::drawBoard(window,state->get_state_info().board);
                    vector<vector<int>> pieceMap = updatePieceMapWithShapesRemaining(state->get_state_info().player1Shapes, state->get_state_info().player2Shapes);
                    ScreenRenderer::drawUnplayedPieces(window, pieceMap);
                    window.display();
                    
                    validPlacement = true; 
                    break;

                } 
                // if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { //this skip is broken since we are 
                //     //changing the state and not the game tree therefore the state and tree will diverge.
                //     // with the tree thinking the root (current state) is still the same turn and with the last move 
                //     state->change_turn();
                //     validPlacement = true; 
                //     turnComplete = true;
                //     break;
                // }

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
                                validPlacement = true;
                                turnComplete = true;
                            
                        
                        }
                    
                }
            }
        }
    }

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


//converst mouse pos to top left cell where the shap would be place. accounting for first top left square
std::pair<int, int> PlayerTurn::convertMousePosToGridCoords(const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld) {
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


vector<vector<int>> PlayerTurn::getUserSelectedPiece(sf::RenderWindow& window, sf::Event& event,
                                      const vector<vector<int>>& pieceMap)
{
    while (true) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return {}; // signal cancel
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosWindow);

                int gridX = static_cast<int>(mousePosWorld.x) / GRID_SIZE;
                int gridY = static_cast<int>(mousePosWorld.y) / GRID_SIZE;

                if (gridY >= 0 && gridY < static_cast<int>(pieceMap.size()) &&
                    gridX >= 0 && gridX < static_cast<int>(pieceMap[0].size())) {
                    int pieceIndex = pieceMap[gridY][gridX];
                    std::cout << "Selected piece index: " << pieceIndex 
                              << " (Player min: " << _playerMin 
                              << ", max: " << _playerMax << ")" << std::endl;
                    if (pieceIndex <= _playerMax && pieceIndex >= _playerMin) {
                        // Extract the shape matrix for the clicked piece
                        return extractShapeFromPosition(pieceMap, gridX, gridY);
                    }
                }
            }
        }
    }
}


void PlayerTurn::renderTurnPreview(
    sf::RenderWindow& window,
    const Cathedral_state* state,
    const std::vector<std::vector<int>>& shape,
    const sf::Vector2f& mousePosWorld)
{
    sf::Sprite sprite;
    sf::Texture texture;
    texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");
    sprite.setTexture(texture);

    window.clear();
    ScreenRenderer::drawBackground(window);
    vector<vector<int>> pieceMap = updatePieceMapWithShapesRemaining(state->get_state_info().player1Shapes, state->get_state_info().player2Shapes);
    ScreenRenderer::drawUnplayedPieces(window, pieceMap);
    ScreenRenderer::drawBoard(window, state->get_state_info().board);

    auto posList = PieceManipulator::getPolyomioPositions(shape, mousePosWorld);
    for (const auto& pos : posList) {
        sprite.setPosition(pos);
        sprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
        window.draw(sprite);
    }

    window.display();
}

