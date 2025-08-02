#include "Headers/GameManager.h"
#include "Headers/ScreenRenderer.h"
#include "Headers/MatrixUtility.h"
#include "Headers/EvaluationMetric.h"
#include "Headers/PlayerTurn.h"
#include "Headers/ResourceManager.h"
#include <iostream>
#include <thread>
#include <future>
using namespace std;

GameManager::GameManager(sf::RenderWindow& win) : window(win), game_tree(nullptr), winner(0) {
    initialiseGame();
    pieceSprite.setTexture(ResourceManager::getInstance().getTexture("misc"));
}

GameManager::~GameManager() {
    delete game_tree;
}

void GameManager::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
        
        // Frame rate limiting (~60 FPS)
        sf::Time elapsed = frameClock.restart();
        sf::Time sleepTime = sf::milliseconds(16) - elapsed;
        if (sleepTime > sf::Time::Zero)
            sf::sleep(sleepTime);
    }
}

void GameManager::initialiseGame() {
    Cathedral_state* stateTemp = new Cathedral_state();
    Cathedral_move m = generateRandomCathedralMove();
    stateTemp->addShapeToBoard(&m);
    game_tree = new MCTS_tree(new Cathedral_state(*stateTemp));
    delete stateTemp;
    
    // Print instructions for the user
    std::cout << "=== Cathedral Game Controls ===" << std::endl;
    std::cout << "Left Click: Select a piece" << std::endl;
    std::cout << "Right Click: Place selected piece" << std::endl;
    std::cout << "R: Rotate selected piece" << std::endl;
    std::cout << "C: Cancel piece placement" << std::endl;
    std::cout << "M: Let MCTS make a move" << std::endl;
    std::cout << "E: Evaluate current board state" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "Game started! Click on a piece to begin or press 'M' for MCTS turn" << std::endl;
}

void GameManager::handleEvents() {
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }
        
        // Global keyboard shortcuts
        if (event.type == sf::Event::KeyPressed) {
            handleKeyboard();
        }
        
        // State-specific event handling
        switch (currentGameState) {
            case GameState::PIECE_SELECTION:
                handlePieceSelection();
                break;
            case GameState::PIECE_PLACEMENT:
                handlePiecePlacement();
                break;
            case GameState::MCTS_TURN:
                // No user input during MCTS turn
                break;
        }
    }
}

void GameManager::update() {
    // Update mouse position
    sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
    mouseWorldPos = window.mapPixelToCoords(mousePosWindow);
    
    // Update game state
    state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());
    if (!state) {
        std::cerr << "Error: state is nullptr!" << std::endl;
        return;
    }
    
    // Update player information
    _player = state->get_state_info().turn;
    if (_player == 1) {
        _playerMin = player1Min;
        _playerMax = player1Max;
    } else {
        _playerMin = player2Min;
        _playerMax = player2Max;
    }
    
    // State-specific updates
    switch (currentGameState) {
        case GameState::MCTS_TURN:
            // Perform MCTS move and switch back to player turn
            performMCTSMove();
            currentGameState = GameState::PIECE_SELECTION;
            break;
        default:
            break;
    }
}

void GameManager::render() {
    window.clear();
    
    // Always draw the base game
    ScreenRenderer::drawBackground(window);
    ScreenRenderer::drawBoard(window, state->get_state_info().board);
    ScreenRenderer::drawTurnSymbol(window, state->get_state_info().turn);
    
    // State-specific rendering
    switch (currentGameState) {
        case GameState::PIECE_SELECTION:
            renderPieceSelection();
            break;
        case GameState::PIECE_PLACEMENT:
            renderPiecePlacement();
            break;
        case GameState::MCTS_TURN:
            renderPieceSelection(); // Show pieces while MCTS is thinking
            break;
    }
    
    window.display();
}

void GameManager::handlePieceSelection() {
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left) {
        
        auto pieceMap = updatePieceMapWithShapesRemaining(
            state->get_state_info().player1Shapes, 
            state->get_state_info().player2Shapes);
        
        int gridX = static_cast<int>(mouseWorldPos.x) / GRID_SIZE;
        int gridY = static_cast<int>(mouseWorldPos.y) / GRID_SIZE;
        
        if (gridY >= 0 && gridY < static_cast<int>(pieceMap.size()) &&
            gridX >= 0 && gridX < static_cast<int>(pieceMap[0].size())) {
            
            int pieceIndex = pieceMap[gridY][gridX];
            std::cout << "Selected piece index: " << pieceIndex 
                      << " (Player min: " << _playerMin 
                      << ", max: " << _playerMax << ")" << std::endl;
            
            if (isValidPlayerPiece(pieceIndex)) {
                selectedPiece = extractShapeFromPosition(pieceMap, gridX, gridY);
                currentGameState = GameState::PIECE_PLACEMENT;
                std::cout << "Piece selected, entering placement mode" << std::endl;
            } else {
                std::cout << "Please click on a valid piece to start your move." << std::endl;
            }
        }
    }
}

void GameManager::handlePiecePlacement() {
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Right) {
        
        auto [gridCol, gridRow] = PlayerTurn::convertMousePosToGridCoords(selectedPiece, mouseWorldPos);
        Cathedral_move move(gridRow - minRow, gridCol - minCol, selectedPiece);
        
        bool noCollision = state->legal_move(&move);
        if (noCollision) {     
            game_tree->advance_tree(&move);
            selectedPiece.clear();
            currentGameState = GameState::PIECE_SELECTION;
            
            // Update state pointer before evaluation
            state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());
            if (state) {
                EvaluationMetric::evaluate(*state);
            } else {
                std::cerr << "Error: state is nullptr after move!" << std::endl;
            }
        } else {
            std::cout << "Invalid placement, try again" << std::endl;
        }
    }
}

void GameManager::handleKeyboard() {
    switch (event.key.code) {
        case sf::Keyboard::C:
            if (currentGameState == GameState::PIECE_PLACEMENT) {
                selectedPiece.clear();
                currentGameState = GameState::PIECE_SELECTION;
                std::cout << "Cancelled piece placement" << std::endl;
            }
            break;
        case sf::Keyboard::R:
            if (currentGameState == GameState::PIECE_PLACEMENT && !selectedPiece.empty()) {
                selectedPiece = rotateMatrix(selectedPiece);
                std::cout << "Rotated piece" << std::endl;
            }
            break;
        case sf::Keyboard::M:
            if (currentGameState == GameState::PIECE_SELECTION) {
                currentGameState = GameState::MCTS_TURN;
                std::cout << "Starting MCTS turn..." << std::endl;
            }
            break;
        case sf::Keyboard::E:
            EvaluationMetric::evaluate(*state);
            break;
        default:
            break;
    }
}

void GameManager::renderPieceSelection() {
    auto pieceMap = updatePieceMapWithShapesRemaining(
        state->get_state_info().player1Shapes, 
        state->get_state_info().player2Shapes);
    ScreenRenderer::drawUnplayedPieces(window, pieceMap);
}

void GameManager::renderPiecePlacement() {
    auto pieceMap = updatePieceMapWithShapesRemaining(
        state->get_state_info().player1Shapes, 
        state->get_state_info().player2Shapes);
    ScreenRenderer::drawUnplayedPieces(window, pieceMap);

    // Draw the piece preview at mouse position
    if (!selectedPiece.empty()) {
        auto posList = PieceManipulator::getPolyomioPositions(selectedPiece, mouseWorldPos);
        for (const auto& pos : posList) {
            pieceSprite.setPosition(pos);
            pieceSprite.setTextureRect(sf::IntRect(GRID_SIZE * _player, 0, GRID_SIZE, GRID_SIZE));
            window.draw(pieceSprite);
        }
    }
}

void GameManager::performMCTSMove() {
    if (winner != 0) {
        cout << "Game has finished." << endl;
        return;
    }
    
    double max_seconds = 10;
    double max_iterations = 51000;
    bool activateRootMCTS = false;
    MCTS_node* best_child = nullptr;

    if (!activateRootMCTS) {
        game_tree->grow_tree(max_iterations, max_seconds);
        best_child = game_tree->select_best_child();
    } else {
        Cathedral_state* copy = new Cathedral_state(*state);
        vector<MCTS_tree*> trees;
        for (int i = 0; i < 4; ++i)
            trees.push_back(new MCTS_tree(copy));

        vector<future<void>> futures;
        for (auto& tree : trees)
            futures.push_back(async(launch::async, [&tree, max_iterations, max_seconds]() {
                tree->grow_tree(max_iterations, max_seconds);
            }));
        for (auto& future : futures) future.get();

        double best_rate = -1;
        for (auto tree : trees) {
            MCTS_node* child = tree->select_best_child();
            if (child) {
                double rate = child->calculate_winrate(state->player1_turn());
                if (rate > best_rate) {
                    best_child = child;
                    best_rate = rate;
                }
            }
        }

        for (auto tree : trees) delete tree;
        delete copy;
    }

    if (best_child) {
        const Cathedral_move* move = (const Cathedral_move*)best_child->get_move();
        game_tree->advance_tree(move);
        
        // Update state pointer after MCTS move
        state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());
        if (state) {
            winner = state->check_winner();
        } else {
            std::cerr << "Error: state is nullptr after MCTS move!" << std::endl;
        }
    } else {
        cout << "No valid move found." << endl;
    }
}

bool GameManager::isValidPlayerPiece(int pieceIndex) const {
    return pieceIndex >= _playerMin && pieceIndex <= _playerMax;
}

int GameManager::getCurrentPlayer() const {
    return _player;
}
