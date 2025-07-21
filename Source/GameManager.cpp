#include "Headers/GameManager.h"
#include "Headers/ScreenRenderer.h"
#include "Headers/MatrixUtility.h"
#include "Headers/EvaluationMetric.h"
#include <iostream>
#include <thread>
#include <future>
using namespace std;

GameManager::GameManager(sf::RenderWindow& win) : window(win), game_tree(nullptr), winner(0) {
    initialiseGame();
}

GameManager::~GameManager() {
    delete game_tree;
}

void GameManager::run() {
    while (window.isOpen()) {
        window.clear();
        state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());
        if (!state) {
            std::cerr << "Error: state is nullptr!" << std::endl;
            // Handle error, skip drawing/evaluation, etc.
            return;
        }
        ScreenRenderer::drawBackground(window);
        ScreenRenderer::drawBoard(window, state->get_state_info().board);
        vector<vector<int>> pieceMap = updatePieceMapWithShapesRemaining(state->get_state_info().player1Shapes, state->get_state_info().player2Shapes);
        ScreenRenderer::drawUnplayedPieces(window, pieceMap);
        window.display();

        if (!processEvents()) break;
    }
}

void GameManager::initialiseGame() {
    Cathedral_state* stateTemp = new Cathedral_state();

    Cathedral_move m = generateRandomCathedralMove();
    stateTemp->addShapeToBoard(&m);
    game_tree = new MCTS_tree(new Cathedral_state(*stateTemp));
}

bool GameManager::processEvents() {
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::M:
                    if (winner == 0) {
                        performMCTSMove();
                        winner = state->check_winner();
                    } else {
                        cout << "Game has finished." << endl;
                    }
                    break;
                case sf::Keyboard::P:
                    performPlayerMove();
                    EvaluationMetric::evaluate(*state); 
                    break;
                case sf::Keyboard::R:
                    // rollout
                    break;
                case sf::Keyboard::A:
                    // auto game
                    break;
                case sf::Keyboard::E:
                    EvaluationMetric::evaluate(*state);
                    break; 
                default:
                    break;
            }
        }
    }
    return true;
}

void GameManager::performPlayerMove() {
    PlayerTurn player;
    player.turn(window, event, game_tree);
    state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());

}

void GameManager::performMCTSMove() {
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
    } else {
        cout << "No valid move found." << endl;
    }
    state = dynamic_cast<const Cathedral_state*>(game_tree->get_current_state());

}
