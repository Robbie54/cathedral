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
    
    std::vector<std::vector<int>> pieceMap = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 2, 0, 3, 0, 4, 4, 0, 5, 5, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 17, 0, 18, 18, 0, 19, 19, 0, 20, 20, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0},
    {0, 7, 7, 0, 8, 8, 8, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 0, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 7, 0, 0, 0, 0, 0, 9, 9, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 23, 23, 23, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 12, 12, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 23, 0, 0, 0, 0, 0},
    {0, 0, 11, 11, 0, 0, 12, 12, 0, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 11, 11, 0, 0, 0, 12, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 0, 0, 0, 26, 26, 0, 27, 27, 27, 0, 0},
    {0, 14, 14, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 0, 0},
    {0, 0, 14, 14, 0, 15, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 26, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 14, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 0, 29, 29, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 29, 29, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }; 

    

    // cout << "Welcome to cathedral! " << endl << "Controls: " << endl << " - Left click to select a piece" << endl << " - Right click to place " << endl << " - R to rotate " << endl << " - C to cancel move " << endl << " - S to skip turn " << endl << endl << "Player 1's turn! " << endl;

    sf::Event event;

	sf::RenderWindow window(sf::VideoMode(GRID_SIZE * SCREEN_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT) * SCREEN_RESIZE), "Cathedral", sf::Style::Close);
	//Resizing the window.
	window.setView(sf::View(sf::FloatRect(0, 0, GRID_SIZE * SCREEN_WIDTH, FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT)));

    addCathedral(pieceMap);

    int playerTurn = 1;
    int turnCounter = 1; //player 1 is odd player 2 is even 
    int winner = 0;
    //mcts setup 
    Cathedral_state *state = new Cathedral_state();
    
    MCTS_tree *game_tree = new MCTS_tree(new Cathedral_state());    // Important: do not use the same state that we change in main loop
    

    while(window.isOpen()){
        while (window.pollEvent(event)){
            window.clear();
            drawBoard(window);
            drawPieces(window, pieceMap);
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
                        double max_seconds = 10; //thinking time
                        double max_iterations = 10000;
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
                    
                    player.turn(window, event, pieceMap, playerTurn, turnCounter); 

                    turnCounter++;

                    playerTurn = (playerTurn == 1) ? 2 : 1;

                    window.display();
                }
            }
    }

}

    delete state;
    delete game_tree;
    return 0;
}





// //!player version


// // int main(){
// //     std::vector<std::vector<int>> pieceMap = {
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
// //     {0, 2, 0, 3, 0, 4, 4, 0, 5, 5, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 17, 0, 18, 18, 0, 19, 19, 0, 20, 20, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0},
// //     {0, 7, 7, 0, 8, 8, 8, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 0, 22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 7, 0, 0, 0, 0, 0, 9, 9, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 23, 23, 23, 0, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 12, 12, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 23, 0, 0, 0, 0, 0},
// //     {0, 0, 11, 11, 0, 0, 12, 12, 0, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0},
// //     {0, 11, 11, 0, 0, 0, 12, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 0, 0, 0, 26, 26, 0, 27, 27, 27, 0, 0},
// //     {0, 14, 14, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 0, 0},
// //     {0, 0, 14, 14, 0, 15, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 26, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 0, 14, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 28, 0, 29, 29, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 29, 29, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
// //     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
// //     }; 

    

// //     // cout << "Welcome to cathedral! " << endl << "Controls: " << endl << " - Left click to select a piece" << endl << " - Right click to place " << endl << " - R to rotate " << endl << " - C to cancel move " << endl << " - S to skip turn " << endl << endl << "Player 1's turn! " << endl;

// //     sf::Event event;

// // 	sf::RenderWindow window(sf::VideoMode(GRID_SIZE * SCREEN_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT) * SCREEN_RESIZE), "Cathedral", sf::Style::Close);
// // 	//Resizing the window.
// // 	window.setView(sf::View(sf::FloatRect(0, 0, GRID_SIZE * SCREEN_WIDTH, FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT)));

// //     addCathedral(pieceMap);

// //     int playerTurn = 1;
// //     int turnCounter = 1; //player 1 is odd player 2 is even 

// //     while(window.isOpen()){
// //         while (window.pollEvent(event))
// //                 {

// //                     if (event.type == sf::Event::Closed) {
// //                         window.close();
// //                     }
                
//                     //     if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P){
//                     //     std::cout << "{\n";
//                     //     for (const auto& row : pieceMap) {
//                     //         std::cout << "    {";
//                     //         for (size_t i = 0; i < row.size(); ++i) {
//                     //             std::cout << row[i];
//                     //             if (i < row.size() - 1) {
//                     //                 std::cout << ", ";
//                     //             }
//                     //         }
//                     //         std::cout << "},\n";
//                     //     }
//                     //     std::cout << "};\n";
//                     // }

// //         window.clear();
// //         drawBoard(window);
// //         drawPieces(window, pieceMap);

// //         player_turn player; 
// //         window.display();

// //         player.turn(window, event, pieceMap, playerTurn, turnCounter); 

// //         turnCounter++;
        

// //         playerTurn = (playerTurn == 1) ? 2 : 1;


// //         window.display();
// //         }
// //     }

// // }













