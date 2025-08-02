#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "CathedralState.h"
#include "PlayerTurn.h"


class GameManager {
public:
    GameManager(sf::RenderWindow& window);
    ~GameManager();

    void run();

private:
    // Initialization
    void initialiseGame();
    
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
    
};
