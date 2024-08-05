#pragma once 

#include <vector>
// #include <iostream>
#include <utility>


#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"

class mcts { 

public: 
    std::pair<int, int> calculateScore(const std::vector<std::vector<int>>& matrix);



private:
    


};

struct Cathedral_move : public MCTS_move {
    int row,col; //top left 
    std::vector<std::vector<int>> shape; //single piece map 
    int shapeIndex;

    Cathedral_move(int r, int c, std::vector<std::vector<int>> s, int shapeIndex)
        : row(r), col(c), shape(s), shapeIndex(shapeIndex) {}

    bool operator==(const MCTS_move& other) const override { 
        const Cathedral_move &o = (const Cathedral_move &) other;        // Note: Casting necessary
        return row == o.row && col == o.col && shape == o.shape;
    };
};

class Cathedral_state : public MCTS_state {
private:
    std::vector<std::vector<int>> board;
    int winner, turn;
    std::vector<std::vector<std::vector<int>>> player1Shapes;
    std::vector<std::vector<std::vector<int>>> player2Shapes;


    void addShapeToBoard(const Cathedral_move *move);
    void change_turn();
    bool canPlaceShapeAtPos(const vector<vector<int>>& shape, int startRow, int startCol) const;
    Cathedral_move *pick_semirandom_move(Cathedral_state &s) const;

    bool legal_move(const Cathedral_move *move);
    
    double evaluate_position(Cathedral_state &s) const;

public:
    Cathedral_state(); 
    Cathedral_state(const Cathedral_state &other);
   
    queue<MCTS_move *> *actions_to_try() const override;
    MCTS_state *next_state(const MCTS_move *move) const override;
    double rollout() const override;                        // the rollout simulation in MCTS
    bool is_terminal() const override; 
    void print() const override; 
    bool player1_turn() const override {return true;}

    //0 is no one 1 and 2 is player //calculated each time state is constructed 
    int check_winner() const;

    bool play_move(const Cathedral_move *move);

   


};





//things we have for heuristics
//score 
//if piece gets removed 
//win/loss
//


//functions 
//random move 
    //maybe have a list of pieces to select 
    //random rotate 
//check winner
    //slow to check board every move for valid plays 
    //calculate score says who wins just need valid plays 