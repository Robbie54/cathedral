#include <SFML/Graphics.hpp>

#include "Headers/global.h"
#include "Headers/draw_board.h"

#include <utility>

void drawBoard(sf::RenderWindow& window){
        sf::Sprite sprite;

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/Capstone/cathedral/Source/Images/misc.png");

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


void drawPieces(sf::RenderWindow& window, std::vector<std::vector<int>>& pieceMap ){ 
        sf::Sprite sprite;

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/Capstone/cathedral/Source/Images/misc.png");
        

        sprite.setTexture(texture);

        
        int player; //corresponds to png positions
        for(int row = 0; row < pieceMap.size(); row++){
            for(int column = 0; column < pieceMap[row].size(); column++){
                if (pieceMap[row][column] == cathedral ){
                  player = 3; 
                }
                else if(pieceMap[row][column] >= player1Min && pieceMap[row][column] <= player1Max){
                  player = 1; 
                }
                else {
                  player = 2;
                }
                

                if(pieceMap[row][column] == 0) {
                    continue; 
                }
                 
                else { 
                 
                        sprite.setPosition(static_cast<float>(GRID_SIZE * column), static_cast<float>(GRID_SIZE * row));
                        sprite.setTextureRect(sf::IntRect(GRID_SIZE * player, 0, GRID_SIZE, GRID_SIZE));
                        window.draw(sprite);
                    
                }
                
            }
        }
       
}
      
