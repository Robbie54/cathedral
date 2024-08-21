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

struct CathedralStateInfo {
    std::vector<std::vector<int>> board;
    int winner;
    int turn;
    std::vector<std::vector<std::vector<int>>> player1Shapes;
    std::vector<std::vector<std::vector<int>>> player2Shapes;
};


struct Cathedral_move : public MCTS_move {
    int row,col; //top left 
    std::vector<std::vector<int>> shape; //single piece map 

    Cathedral_move(int r, int c, std::vector<std::vector<int>> s)
        : row(r), col(c), shape(s) {}

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
    bool player1_turn() const override {return turn == 1;} //return true if 1 

    //0 is no one 1 and 2 is player //calculated each time state is constructed 
    int check_winner() const;

    bool play_move(const Cathedral_move *move);

     CathedralStateInfo get_state_info() const {
        return { board, winner, turn, player1Shapes, player2Shapes };
    }

    vector<vector<int>> updatePieces(int player);

    
    int containsInt(const std::vector<std::vector<std::vector<int>>>& vec, int target);
    void addShapeToBoard(const Cathedral_move *move);
    int change_turn();





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