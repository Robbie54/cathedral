#include "Headers/GameManager.h"
#include "Headers/ScreenRenderer.h"
#include "Headers/MatrixUtility.h"
#include "Headers/EvaluationMetric.h"
#include "Headers/PlayerTurn.h"
#include "Headers/BoardUtility.h"
#include <iostream>
#include <thread>
#include <future>
using namespace std;

GameManager::GameManager(sf::RenderWindow& win) : window(win), game_tree(nullptr), winner(0) {
    initialiseGame();
    loadResources();
}

GameManager::~GameManager() {
    delete game_tree;
}

void GameManager::loadResources() {
    // Load textures once at startup
    pieceTexture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");
    pieceSprite.setTexture(pieceTexture);
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
    std::cout << "D: Toggle territory debug visualization" << std::endl;
    std::cout << "T: Test territory checking with debug (enable D first)" << std::endl;
    std::cout << "E: Evaluate current position" << std::endl;
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
    
    // Draw debug visualization if enabled
    if (debugTerritoryMode && debugBoardUtil) {
        debugBoardUtil->drawVisitedPositions();
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
            // If debug mode is enabled, test territory checking for the move that was just made
            if (debugTerritoryMode) {
                std::cout << "Debug: Testing territory for the move that was just placed..." << std::endl;
                testTerritoryForMove(move);
            }
            
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
        case sf::Keyboard::D:
            debugTerritoryMode = !debugTerritoryMode;
            if (debugTerritoryMode) {
                std::cout << "Territory debug visualization enabled - red squares will show visited positions during territory checking" << std::endl;
            } else {
                std::cout << "Territory debug visualization disabled" << std::endl;
                debugBoardUtil.reset(); // Clear the debug BoardUtility
                stepByStepFloodFillMode = false; // Reset step-by-step mode
            }
            break;
        case sf::Keyboard::T:
            if (debugTerritoryMode) {
                testTerritoryDebug();
            } else {
                std::cout << "Enable debug mode first (press D), then press T to test territory checking" << std::endl;
            }
            break;
        case sf::Keyboard::N:
            if (debugTerritoryMode && stepByStepFloodFillMode && debugBoardUtil) {
                if (debugBoardUtil->hasMoreFloodFills()) {
                    debugBoardUtil->nextFloodFill();
                } else {
                    std::cout << "No more flood fills to display. Place a piece to generate new flood fills." << std::endl;
                }
            } else if (debugTerritoryMode && !stepByStepFloodFillMode) {
                std::cout << "Step-by-step mode not enabled. Place a piece first to analyze territory." << std::endl;
            } else {
                std::cout << "Enable debug mode first (press D), then place a piece to analyze territory step-by-step." << std::endl;
            }
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

void GameManager::testTerritoryDebug() {
    if (!state) {
        std::cout << "No game state available for testing" << std::endl;
        return;
    }
    
    std::cout << "Testing territory checking with debug visualization..." << std::endl;
    std::cout << "Creating a test move to check territory. Check the game window for red squares showing visited positions." << std::endl;
    
    // Get current board and player info
    auto board = state->get_state_info().board;
    int currentPlayer = state->get_state_info().turn;
    int playerTerritory = (currentPlayer == 1) ? player1Territory : player2Territory;
    int opponentTerritory = (currentPlayer == 1) ? player2Territory : player1Territory;
    
    // Create a BoardUtility instance with debug enabled
    debugBoardUtil = std::make_unique<BoardUtility>(currentPlayer, board);
    debugBoardUtil->setTerritoryInfo(playerTerritory, opponentTerritory);
    debugBoardUtil->enableDebugVisualization(&window);
    
    // Create a simple test move (1x1 piece at center of board for testing)
    std::vector<std::vector<int>> testShape = {{currentPlayer == 1 ? 2 : 26}}; // Simple 1x1 piece
    Cathedral_move testMove(5, 5, testShape); // Place at position (5,5) which should be on the board
    
    std::cout << "Testing with move at position (5,5) for player " << currentPlayer << std::endl;
    std::cout << "Using piece value: " << (currentPlayer == 1 ? 2 : 26) << std::endl;
    
    // Test territory checking - this will populate visitedPositions
    bool territoryCreated = debugBoardUtil->checkIfCreatingTerritory(&testMove);
    
    std::cout << "Territory checking complete. Result: " << (territoryCreated ? "Territory created" : "No territory created") << std::endl;
    
    // Check if any positions were visited for debugging
    std::cout << "Debug: Checking if positions were collected..." << std::endl;
    std::cout << "Red squares in the game window show all positions visited during the flood-fill algorithm." << std::endl;
    std::cout << "The visualization will persist until you disable debug mode (press D again)." << std::endl;
}

void GameManager::testTerritoryForMove(const Cathedral_move& move) {
    if (!state) {
        std::cout << "No game state available for testing" << std::endl;
        return;
    }
    
    std::cout << "Debug: Testing territory for actual move at position (" << move.row << ", " << move.col << ")" << std::endl;
    
    // Get current board and player info (before the move is applied)
    auto board = state->get_state_info().board;
    int currentPlayer = state->get_state_info().turn;
    int playerTerritory = (currentPlayer == 1) ? player1Territory : player2Territory;
    int opponentTerritory = (currentPlayer == 1) ? player2Territory : player1Territory;
    
    // Create a BoardUtility instance with debug enabled
    debugBoardUtil = std::make_unique<BoardUtility>(currentPlayer, board);
    debugBoardUtil->setTerritoryInfo(playerTerritory, opponentTerritory);
    debugBoardUtil->enableDebugVisualization(&window);
    debugBoardUtil->enableStepByStepMode(); // Enable step-by-step flood fill mode
    stepByStepFloodFillMode = true;
    
    // Test territory checking for the actual move - this will populate individual flood fills
    bool territoryCreated = debugBoardUtil->checkIfCreatingTerritory(&move);
    
    std::cout << "Debug: Territory checking complete for move. Result: " << (territoryCreated ? "Territory created" : "No territory created") << std::endl;
    if (debugBoardUtil->getTotalFloodFills() > 0) {
        std::cout << "Debug: Captured " << debugBoardUtil->getTotalFloodFills() << " flood fills. Press N to step through them!" << std::endl;
        std::cout << "Debug: Currently showing flood fill 1/" << debugBoardUtil->getTotalFloodFills() << std::endl;
    } else {
        std::cout << "Debug: No flood fills were captured" << std::endl;
    }
}
