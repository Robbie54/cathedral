#pragma once 

#include <vector>
#include <utility>
#include <random>

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"


struct CathedralStateInfo {
    std::vector<std::vector<int>> board;
    int winner;
    int turn;
    std::vector<std::vector<std::vector<int>>> player1Shapes;
    std::vector<std::vector<std::vector<int>>> player2Shapes;
    
};

struct Cathedral_move : public MCTS_move {
    int row,col; //top left position of the shape/piece map
    std::vector<std::vector<int>> shape; //single piece map 

    Cathedral_move(int r, int c, std::vector<std::vector<int>> s)
        : row(r), col(c), shape(s) {}

    bool operator==(const MCTS_move& other) const override { 
        const Cathedral_move &o = (const Cathedral_move &) other;        // Note: Casting necessary
        return row == o.row && col == o.col && shape == o.shape;
    };
    string sprint() const override {
        std::string result;
        for (const auto& row : shape) {
            result += "\n"; 
            for (int val : row) {
                result += std::to_string(val) + " "; 
            }
            
        }
        int shapeNum; 
        if(shape[0][0] != 0){
            shapeNum = shape[0][0];
        }
        else{
            if(shape[0].size() > 1){
                shapeNum = shape[0][1];
            }
        }
        
        return "shape " + to_string(shapeNum) +  " at row: " + to_string(row) + " col: " + to_string(col);
    }
};

class Cathedral_state : public MCTS_state {
private:
    std::vector<std::vector<int>> board;
    int winner, turn;
    std::vector<std::vector<std::vector<int>>> player1Shapes;
    std::vector<std::vector<std::vector<int>>> player2Shapes;
    int playerMin, playerMax;
    int opponentMin, opponentMax;
    int playerTerritory, opponentTerritory;
    
    const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool firstTurn;
    int pieceNumToRemove;


    Cathedral_move *pick_semirandom_move(Cathedral_state &s) const;

    
    double evaluate_position(Cathedral_state &s) const;

    Cathedral_move *pickRandomMove(Cathedral_state &s) const;


    //Territory stuff
    bool checkIfCreatingTerritory(const Cathedral_move *move);
    bool checkIfPositionIsNowPlayersTerritory(int row, int col, std::vector<std::vector<bool>>& visited);

    std::vector<std::pair<int, int>> positionsAroundShape(const std::vector<std::vector<int>>& shape);
    void changeSpaceToPlayersTerritory(int row, int col);

    void addShapeToPlayerShapes(int pieceNum);

    //static so same across all states
    static default_random_engine generator;

    bool any_actions_to_try() const;

    vector<Cathedral_move> movesVec; 

public:
    Cathedral_state(); 
    Cathedral_state(const Cathedral_state &other);
   
    queue<MCTS_move *> *actions_to_try() const override;
    MCTS_state *next_state(const MCTS_move *move) const override;
    double rollout() const override;                        // the rollout simulation in MCTS
    bool is_terminal() const override; 
    void print() const override; 
    //virtual of mcts 
    bool player1_turn() const override {return turn == 1;} //return true if 1 

    bool legal_move(const Cathedral_move *move) const;

    //0 is no one 1 and 2 is player //calculated each time state is constructed 
    int check_winner() const;

    bool play_move(const Cathedral_move *move);

     CathedralStateInfo get_state_info() const {
        return { board, winner, turn, player1Shapes, player2Shapes };
    }
    
    int containsInt(const std::vector<std::vector<std::vector<int>>>& vec, int target);
    void addShapeToBoard(const Cathedral_move *move);
    int change_turn();


    void addMove(Cathedral_move m);
    
     





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