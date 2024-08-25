#include <SFML/Graphics.hpp>


#include "Headers/draw_board.h"
#include "Headers/global.h"
#include "Headers/player_turn.h"
#include "Headers/mcts.h"
#include "Headers/matrix_utility.h"
#include "../MonteCarloTreeSearch-main/mcts/include/mcts.h"



#include <iostream>

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

    const Cathedral_move m = addCathedral(blankBoard);
    
    state->addShapeToBoard(&m);

    MCTS_tree *game_tree = new MCTS_tree(new Cathedral_state(*state));    // Important: do not use the same state that we change in main loop 
                                                                        //im creating a copy here i think 

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
                    cout << "MCTS " << endl;
                    if(winner != 0){
                        cin.ignore(512, '\n');
                        cout << "Game has already finished." << endl << endl;
                    } else {
                        double max_seconds = 1; //thinking time
                        double max_iterations = 2000;
                        game_tree->grow_tree(max_iterations, max_seconds);
                        game_tree->print_stats(); 
                        
                        
                        MCTS_node *best_child = game_tree->select_best_child();

                        if (best_child == NULL) {
                            cerr << "Warning: Could not find best child. Tree has no children? Possible terminal node" << endl << endl;
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
                    
                    player.turn(window, event, state); 
                    
                }
                if (event.key.code == sf::Keyboard::R) { 
                    cout << "Playing random game" << endl;
                    //could this not just be a single rollout 
                    double result = state->rollout();
                    // winner = state->check_winner();

                    cout << "Random game complete result is: " << result << endl;
                }

            }
    }

}

    delete state;
    delete game_tree;
    return 0;
}















//A typical turn process

//Select a piece 
//Find a valid position -> in board, no collision, not in territory 
//play Move 
//check we didnt create territory -> if we did remove piece and add back to vecotr 
//
