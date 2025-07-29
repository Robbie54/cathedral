#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "CathedralState.h"
#include "PlayerTurn.h"

// Forward declaration
class BoardUtility;

class GameManager {
public:
    GameManager(sf::RenderWindow& window);
    ~GameManager();

    void run();

private:
    // Initialization
    void initialiseGame();
    void loadResources();
    
    // Main game loop components
    void handleEvents();
    void update();
    void render();
    
    // Event handlers
    void handlePieceSelection();
    void handlePiecePlacement();
    void handleKeyboard();
    
    // Rendering methods
    void renderPieceSelection();
    void renderPiecePlacement();
    
    // Game logic
    void performMCTSMove();
    bool isValidPlayerPiece(int pieceIndex) const;
    int getCurrentPlayer() const;
    void testTerritoryDebug();  // Test territory checking with debug visualization
    void testTerritoryForMove(const Cathedral_move& move);  // Test territory for a specific move
    
    // Core game objects
    sf::RenderWindow& window;
    sf::Event event;
    sf::Clock frameClock;
    const Cathedral_state* state;
    MCTS_tree* game_tree;
    int winner;
    
    // Game state management
    enum class GameState { PIECE_SELECTION, PIECE_PLACEMENT, MCTS_TURN };
    GameState currentGameState = GameState::PIECE_SELECTION;
    
    // UI state
    std::vector<std::vector<int>> selectedPiece;
    sf::Vector2f mouseWorldPos;
    
    // Pre-loaded resources
    sf::Texture pieceTexture;
    sf::Sprite pieceSprite;
    
    // Player state
    int _player;
    int _playerMin;
    int _playerMax;
    
    // Debug state
    bool debugTerritoryMode = false;
    std::unique_ptr<BoardUtility> debugBoardUtil;
    bool stepByStepFloodFillMode = false;
};
