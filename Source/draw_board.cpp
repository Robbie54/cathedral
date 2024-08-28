#include <SFML/Graphics.hpp>

#include "Headers/global.h"
#include "Headers/draw_board.h"
#include "Headers/mcts.h"


#include <utility>

void drawBackground(sf::RenderWindow& window){
        sf::Sprite sprite;

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

        sprite.setTexture(texture);
      
          for (unsigned int a = 0; a < SCREEN_WIDTH; a++)
            {
                for (unsigned int b = 0; b < SCREEN_HEIGHT; b++)
                {  
   
                  if(b < SCREEN_HEIGHT/2-BOARD_SIZE/2 || b >= SCREEN_HEIGHT/2+BOARD_SIZE/2 || a < SCREEN_WIDTH/2-BOARD_SIZE/2 || a >= SCREEN_WIDTH/2+BOARD_SIZE/2){
                     sprite.setPosition(static_cast<float>(GRID_SIZE * a), static_cast<float>(GRID_SIZE * b));
                      sprite.setTextureRect(sf::IntRect(0, GRID_SIZE, GRID_SIZE, GRID_SIZE));

                      window.draw(sprite);
                  }

                    
                  else{

                    sprite.setPosition(static_cast<float>(GRID_SIZE * a), static_cast<float>(GRID_SIZE * b));
                    sprite.setTextureRect(sf::IntRect(0, 0, GRID_SIZE, GRID_SIZE));

                    window.draw(sprite);
                  }
                }

            }
    }

void drawBoard(sf::RenderWindow& window, Cathedral_state* state){
        sf::Sprite sprite;

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

        sprite.setTexture(texture);

        vector<vector<int>> board = state->get_state_info().board;

        for(int i = 0; i < board.size(); i++){
          for(int j = 0; j < board[i].size(); j++){
            if(board[i][j] == 0){
              continue;
            }
            else if(board[i][j] == cathedral){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + minCol * GRID_SIZE), static_cast<float>(GRID_SIZE * i + minRow * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE * 3, 0, GRID_SIZE, GRID_SIZE));
              window.draw(sprite);
            }
            else if(board[i][j] >= player1Min && board[i][j] <= player1Max){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + minCol * GRID_SIZE), static_cast<float>(GRID_SIZE * i + minRow * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE, 0, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j]>= player2Min && board[i][j] <= board[i][j]){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + minCol * GRID_SIZE), static_cast<float>(GRID_SIZE * i + minRow * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE * 2, 0, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j] == player1Territory){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + minCol * GRID_SIZE), static_cast<float>(GRID_SIZE * i + minRow * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE, GRID_SIZE, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j] == player2Territory){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + minCol * GRID_SIZE), static_cast<float>(GRID_SIZE * i + minRow * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE*2, GRID_SIZE, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            
            

          }
        }
      
          

    }
    



void drawPieces(sf::RenderWindow& window, Cathedral_state* state){ 

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");
        
        int player = 1; //corresponds to png positions
        
        sf::Sprite sprite;
        sprite.setTexture(texture);

        vector<vector<int>> player1Pieces = state->updatePieces(1);
        vector<vector<int>> player2Pieces = state->updatePieces(2);

        for(int row = 0; row < player1Pieces.size(); row++){
            for(int column = 0; column < player1Pieces[row].size(); column++){
                 if(player1Pieces[row][column] == 0) {
                    continue; 
                }
                 
                else { 
                        
                        sprite.setPosition(static_cast<float>(GRID_SIZE * (column + 1)), static_cast<float>(GRID_SIZE * (row+1)));
                        sprite.setTextureRect(sf::IntRect(GRID_SIZE * player, 0, GRID_SIZE, GRID_SIZE));
                        
                        window.draw(sprite);
                }
                
            }
        }


        //board 


        //player 2
      player = 2;
        for(int row = 0; row < player2Pieces.size(); row++){
          for(int column = 0; column < player2Pieces[row].size(); column++){
                if(player2Pieces[row][column] == 0) {
                  continue; 
              }
                
              else { 
                   
                      sprite.setPosition(static_cast<float>(GRID_SIZE * (column +2) + GRID_SIZE*maxCol), static_cast<float>(GRID_SIZE * (row+1)));
                      sprite.setTextureRect(sf::IntRect(GRID_SIZE * player, 0, GRID_SIZE, GRID_SIZE));
                      window.draw(sprite);
                }
                
            }
        }
     
}
      

