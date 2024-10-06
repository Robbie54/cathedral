#include <SFML/Graphics.hpp>


#include "Headers/draw_board.h"
#include "Headers/global.h"
#include "Headers/player_turn.h"
#include "Headers/mcts.h"
#include "Headers/matrix_utility.h"
#include "../MonteCarloTreeSearch-main/mcts/include/mcts.h"



#include <iostream>
#include <thread>
#include <future>

using namespace std;



int main(){

    // cout << "Welcome to cathedral! " << endl << "Controls: " << endl << " - Left click to select a piece" << endl << " - Right click to place " << endl << " - R to rotate " << endl << " - C to cancel move " << endl << " - S to skip turn " << endl << endl << "Player 1's turn! " << endl;

    sf::Event event;

	sf::RenderWindow window(sf::VideoMode(GRID_SIZE * SCREEN_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT) * SCREEN_RESIZE), "Cathedral", sf::Style::Close);
	//Resizing the window.
	window.setView(sf::View(sf::FloatRect(0, 0, GRID_SIZE * SCREEN_WIDTH, FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT)));
    window.setPosition(sf::Vector2i(100, 100));

    int winner = 0;
    //mcts setup 
    Cathedral_state *state = new Cathedral_state();
    

    vector<vector<int>> blankBoard(10, std::vector<int>(10, 0));

    MCTS_tree *game_tree;

    cout << "Do you want to add cathedral? y/n" << endl;

    bool waitingForKeyPress = true;
    while (waitingForKeyPress) {
        if (window.waitEvent(event)) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Y){
                    const Cathedral_move m = addCathedral(blankBoard);
                    state->addShapeToBoard(&m);
                    game_tree = new MCTS_tree(new Cathedral_state(*state));    // Important: do not use the same state that we change in main loop 
                    waitingForKeyPress = false; 
                }
                if (event.key.code == sf::Keyboard::N){
                    waitingForKeyPress = false; 
                }
            }
        }
    }
   
    while(window.isOpen()){
        while (window.pollEvent(event)){
            window.clear();
            drawBackground(window);
            drawBoard(window, state);
            drawPieces(window, state);

            window.display();
           
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {

                if (event.key.code == sf::Keyboard::M) {
                    
                   
                    if(winner != 0){
                        cin.ignore(512, '\n');
                        cout << "Game has already finished." << endl << endl;
                    } else {
                        cout << "Root MCTS " << endl;
                        double max_seconds = 60; //thinking time
                        double max_iterations = 50000;

                        bool activateRootMCTS = true;
                        MCTS_node *best_child;
                        if(!activateRootMCTS){
                            cout << "Sequential MCTS" << endl;
                            game_tree->grow_tree(max_iterations, max_seconds);
                            game_tree->print_stats_cathedral(); 
                            
                            
                            best_child = game_tree->select_best_child();
                            if (best_child == NULL) {
                                cerr << "Warning: Could not find best child. Tree has no children? Possible terminal node" << endl << endl;
                            }
                        }
                        else{
                            Cathedral_state *s = new Cathedral_state(*state);

                            std::vector<MCTS_tree*> trees;
                            int num_trees = 4; 
                            for (int i = 0; i < num_trees; ++i) {
                                trees.push_back(new MCTS_tree(s)); 
                            }

                            vector<future<void>> futures;
                            for (auto& tree : trees) {
                                futures.push_back(async(launch::async, [&]() {
                                    tree->grow_tree(max_iterations, max_seconds);
                                }));
                            }

                            // Ensure all threads complete before proceeding
                            for (auto& future : futures) {
                                future.get();
                            }
                        
                            
                            
                            double win_rate = -1;

                            for (auto tree : trees) {
                                tree->print_stats();  
                                MCTS_node *child = tree->select_best_child();
                                if (child != NULL) {
                                    double winRateTemp = child->calculate_winrate(state->player1_turn()); // Assuming you have a method to get win rate
                                    cout << "Win rate is " << winRateTemp << endl;
                                    if(winRateTemp > win_rate){
                                        best_child = tree->select_best_child();
                                    }   
                                }
                            }
                        }
   
                        const Cathedral_move *best_move = (const Cathedral_move *) best_child->get_move();

                         // advance the tree so the selected child node is now the root
                        game_tree->advance_tree(best_move);
                        
                        // play AI move
                        cout << "playing move in main" << endl;
                        bool succ = state->play_move(best_move);
                        if (!succ) {
                            cerr << "Warning: AI generated illegal move: " <<  best_move->sprint() << endl << endl;
                        } else {
                            // print AI's move
                            cout << best_move->sprint() << endl << endl;
                        }

                        // check if winning move
                        winner = state->check_winner();



                    }
                    
                
                }
                if (event.key.code == sf::Keyboard::P) {
                    cout << "Player move " << endl;
                    
                    player_turn player; 
                    
                    player.turn(window, event, state, game_tree); 
                    
                }
                 
               
                if (event.key.code == sf::Keyboard::R) { 
                      int numberOfRandomGames = 100;
                        double totalResult = 0.0;

                        cout << "Playing random games" << endl;
                        
                        for(int i = 0; i < numberOfRandomGames; i++){
                            totalResult += state->rollout(); //currently these games are not random at all they all play out the same //probaly bc pick semirandom move is not random 
                            cout << "Game " << i << " result is: " << totalResult << endl;
                        }
                        
                        double averageResult = totalResult / numberOfRandomGames;

                        cout << "Random games complete. Average chance of Player winning is " << averageResult << endl;            

                 }
                if (event.key.code == sf::Keyboard::A){
                    std::vector<std::vector<int>> Tavern = {{2}}; 
                    std::vector<std::vector<int>> Tavern2 = {{3}}; 
                    std::vector<std::vector<int>> Stable = {{4, 4}};
                    std::vector<std::vector<int>> Stable2 = {{5, 5}};
                    std::vector<std::vector<int>> Inn = {{6, 6}, {0, 6}};
                    std::vector<std::vector<int>> Inn2 = {{7, 7}, {0, 7}};
                    std::vector<std::vector<int>> Bridge = {{8, 8, 8}};
                    std::vector<std::vector<int>> Square = {{9, 9}, {9, 9}};
                    std::vector<std::vector<int>> Manor = {{10, 10, 10}, {0, 10, 0}};
                    std::vector<std::vector<int>> Abbey = {{0, 11, 11}, {11, 11, 0}};
                    std::vector<std::vector<int>> Academy = {{12, 12, 0}, {0, 12, 12}, {0, 12, 0}};
                    std::vector<std::vector<int>> Infirmary = {{0, 13, 0}, {13, 13, 13}, {0, 13, 0}};
                    std::vector<std::vector<int>> Castle = {{14, 14, 14}, {14, 0, 14}};
                    std::vector<std::vector<int>> Tower = {{0, 15, 15}, {15, 15, 0}, {15, 0, 0}}; 
                    std::vector<std::vector<int>> twoTavern = {{26}}; // Tavern
                    std::vector<std::vector<int>> twoTavern2 = {{27}}; // Tavern2
                    std::vector<std::vector<int>> twoStable = {{28, 28}}; // Stable
                    std::vector<std::vector<int>> twoStable2 = {{29, 29}}; // Stable2
                    std::vector<std::vector<int>> twoInn = {{30, 30}, {0, 30}}; // Inn
                    std::vector<std::vector<int>> twoInn2 = {{31, 31}, {0, 31}}; // Inn2
                    std::vector<std::vector<int>> twoBridge = {{32, 32, 32}}; // Bridge
                    std::vector<std::vector<int>> twoSquare = {{33, 33}, {33, 33}}; // Square
                    std::vector<std::vector<int>> twoManor = {{34, 34, 34}, {0, 34, 0}}; // Manor
                    std::vector<std::vector<int>> twoAbbey = {{0, 35, 35}, {35, 35, 0}}; // Abbey
                    std::vector<std::vector<int>> twoAcademy = {{36, 36, 0}, {0, 36, 36}, {0, 36, 0}}; // Academy
                    std::vector<std::vector<int>> twoInfirmary = {{0, 37, 0}, {37, 37, 37}, {0, 37, 0}}; // Infirmary
                    std::vector<std::vector<int>> twoCastle = {{38, 38, 38}, {38, 0, 38}}; // Castle
                    std::vector<std::vector<int>> twoTower = {{0, 39, 39}, {39, 39, 0}, {39, 0, 0}}; // Tower

                    cout << "Auto game first move only!" << endl;
                    
                    std::vector<std::vector<int>> shape = {{0, cathedral, 0}, {cathedral, cathedral, cathedral}, {0, cathedral, 0}, {0,cathedral,0}};
                    Cathedral_move m(0,0, shape);
                    state->addShapeToBoard(&m);
                    game_tree = new MCTS_tree(new Cathedral_state(*state));    
                    
                    m = Cathedral_move(2,7, Tower);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);

                    m = Cathedral_move(3,3, twoManor);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);

                    m = Cathedral_move(5,5, Castle);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                    m = Cathedral_move(5,0, twoAbbey);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                    m = Cathedral_move(7,0, Academy);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                    m = Cathedral_move(0,4, twoAcademy);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                    m = Cathedral_move(0,7, Abbey);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                    m = Cathedral_move(6,7, twoInfirmary);
                    state->play_move(&m);
                    game_tree->advance_tree(&m);
                }

            }
    }

}

    delete state;
    delete game_tree;
    return 0;
}


