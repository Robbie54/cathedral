#include <SFML/Graphics.hpp>

#include "Headers/Global.h"
#include "Headers/ScreenRenderer.h"
#include "Headers/CathedralState.h"
#include "Headers/MatrixUtility.h"

#include <utility>

void ScreenRenderer::drawBackground(sf::RenderWindow& window){
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

void ScreenRenderer::drawBoard(sf::RenderWindow& window, vector<vector<int>> board, int posX, int posY){
        sf::Sprite sprite;

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

        sprite.setTexture(texture);

        for(int i = 0; i < board.size(); i++){
          for(int j = 0; j < board[i].size(); j++){
            if(board[i][j] == 0){
                sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
                sprite.setTextureRect(sf::IntRect(0, 0, GRID_SIZE, GRID_SIZE));
                window.draw(sprite);    
            }
            else if(board[i][j] == cathedral){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE * 3, 0, GRID_SIZE, GRID_SIZE));
              window.draw(sprite);
            }
            else if(board[i][j] >= player1Min && board[i][j] <= player1Max){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE, 0, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j]>= player2Min && board[i][j] <= player2Max){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE * 2, 0, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j] == player1Territory){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE, GRID_SIZE, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            else if(board[i][j] == player2Territory){
              sprite.setPosition(static_cast<float>(GRID_SIZE * j + posY * GRID_SIZE), static_cast<float>(GRID_SIZE * i + posX * GRID_SIZE));
              sprite.setTextureRect(sf::IntRect(GRID_SIZE*2, GRID_SIZE, GRID_SIZE, GRID_SIZE));

              window.draw(sprite);
            }
            
            

          }
        }
      
          

    }
  
void ScreenRenderer::drawUnplayedPieces(sf::RenderWindow& window, vector<vector<int>> pieceMap){ 

        sf::Texture texture;
        texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");
                
        sf::Sprite sprite;
        sprite.setTexture(texture);

          for(int row = 0; row < pieceMap.size(); row++){
              for(int column = 0; column < pieceMap[row].size(); column++){
                  if(pieceMap[row][column] == 0) {
                      continue; 
                  }

                      int playerTexture = 1;
                      if(pieceMap[row][column] >= player2Min) {
                          playerTexture = 2;
                      }
                      sprite.setPosition(static_cast<float>(GRID_SIZE * column), static_cast<float>(GRID_SIZE * (row)));
                      sprite.setTextureRect(sf::IntRect(GRID_SIZE * playerTexture, 0, GRID_SIZE, GRID_SIZE));
                      window.draw(sprite);                          
                  }

                  
              }
          
     
}

void ScreenRenderer::drawMove(sf::RenderWindow& window, Cathedral_move* move, int posX, int posY){
    sf::Sprite sprite;
    sf::Texture texture;
    texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

    sprite.setTexture(texture);

    for(int x = 0; x < move->shape.size(); x++){
      for(int y = 0; y < move->shape[0].size(); y++){
        if(move->shape[x][y] == 0){
          continue;
        }
        else{
                sprite.setPosition(static_cast<float>(GRID_SIZE * y + posY * GRID_SIZE + move->col * GRID_SIZE), static_cast<float>(GRID_SIZE * x + posX * GRID_SIZE + GRID_SIZE * move->row));
               sprite.setTextureRect(sf::IntRect(GRID_SIZE*3, GRID_SIZE, GRID_SIZE, GRID_SIZE));
                window.draw(sprite);
        }
          
        }

      }
}

void ScreenRenderer::drawTurnSymbol(sf::RenderWindow& window, int player){
  sf::Font font;
  if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
    // Handle error, or return
    return;
  }

  

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(12);
  text.setFillColor(sf::Color::White);
  if(player == 1){
    text.setString("Green's Turn");
  }
  else {
    text.setString("Red's Turn");
  }

  // Position at (minCol * GRID_SIZE, maxRow * GRID_SIZE)
  text.setPosition(static_cast<float>(minCol * GRID_SIZE), static_cast<float>(32 + (maxRow * GRID_SIZE)));

  window.draw(text);
}

void ScreenRenderer::drawSquareClicked(sf::RenderWindow& window, int gridX, int gridY){
  sf::Texture texture;
  texture.loadFromFile("/home/robbie/Desktop/capstone/cathedral/Source/Images/misc.png");

  sf::Sprite sprite;
  sprite.setTexture(texture);
  sprite.setTextureRect(sf::IntRect(GRID_SIZE * 3,0, GRID_SIZE, GRID_SIZE));
  sprite.setPosition(static_cast<float>(gridX * GRID_SIZE), static_cast<float>(gridY * GRID_SIZE));
  window.draw(sprite);
}
