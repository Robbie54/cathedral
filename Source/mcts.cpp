
#include "Headers/global.h"
#include "Headers/mcts.h"
#include "Headers/matrix_utility.h"

#include <vector>
#include <iostream>
#include <utility>

using namespace std;

std::vector<std::vector<std::vector<int>>> shapeFullList{  
    {{1}}, //Tavern
    {{1}},
    {{1, 1}}, //Stable
    {{1, 1}},
    {{1, 1},{0,1}}, //Inn
    {{1,1},{0,1}},
    {{1,1,1}}, //Bridge
    {{1, 1},{1,1}}, //Square
    {{1, 1,1},{0,1,0}}, //Manor
    {{0, 1,1},{1,1,0}}, //Abbey
    {{1, 1,0},{0,1,1}, {0,1,0}}, //Academy
    {{0, 1,0},{1,1,1}, {0,1,0}}, //Infirmary 
    {{1, 1,1},{1,0,1}}, //Castle
    {{0, 1,1},{1,1,0},{1,0,0}}}; //Tower





std::pair<int, int> mcts::calculateScore(const std::vector<std::vector<int>>& matrix){
    int playerOneScore = 0;
    int playerTwoScore = 0;

    for(int i = 0; i < minX; i++){
        for(int j = 0; j < SCREEN_HEIGHT; j++){
            if(matrix[j][i] >= player1Min && matrix[j][i] <= player1Max){
                playerOneScore++;
            }
        }
    }

    for(int i = maxX; i<SCREEN_WIDTH ; i++){
        for(int j = 0; j < SCREEN_HEIGHT; j++){
            if(matrix[j][i] >= player2Min && matrix[j][i] <= player2Max){
                playerTwoScore++;
            }
        }
    }


    if(playerOneScore < playerTwoScore){
        return {1,playerTwoScore-playerOneScore};
    }
    else if(playerOneScore > playerTwoScore){
        return {2,playerOneScore-playerTwoScore};
    }
  
    return {0,0};
}

//in ttt this is called once before do while in quoridor same only for new game
//no winner is calcualted 
Cathedral_state::Cathedral_state() : MCTS_state(), board(10, std::vector<int>(10, 0)), winner(0), turn(1), player1Shapes(shapeFullList), player2Shapes(shapeFullList) {
}

Cathedral_state::Cathedral_state(const Cathedral_state &other) : MCTS_state(other), winner(other.winner), board(other.board), turn(other.turn), player1Shapes(other.player1Shapes), player2Shapes(other.player2Shapes){
   
};

bool Cathedral_state::is_terminal() const {
    int winner = check_winner();
    return winner == 1 || winner == 2 || winner == 3;
}

int Cathedral_state::check_winner() const {

    
    queue<MCTS_move*> *Q = actions_to_try();
    if(Q->empty()){
        delete Q;
        cout << "que empty in check winner " << endl;
        if(player1Shapes.size() > player2Shapes.size()){
            return 2;
        }
        else if(player2Shapes.size() > player1Shapes.size()){
            return 1;
        }
        else{
            return 3; //tie
        }
    }

    return 0;
}


MCTS_state *Cathedral_state::next_state(const MCTS_move *move) const {
    Cathedral_state *new_state = new Cathedral_state(*this);
    new_state->play_move((const Cathedral_move *) move);
    return new_state;

    // Cathedral_move *m = (Cathedral_move *) move;
    // Cathedral_state *new_state = new Cathedral_state(*this); //create new state from current
    // for(int row = 0; row < m->shape.size(); row++){
    //     for(int col = 0; col < m->shape[row].size(); col++){
    //         //if part of shape and board pos is not empty cant update state 
    //         //may need territory to 
    //         if(m->shape[row][col] != 0 && new_state->board[m->row][m->col] != 0 ){
    //             cerr << "Warning: Illegal move (" << m->row << ", " << m->col << ")" << endl;
    //             return NULL;
    //         }
    //     }
    // }
    // //next update board and see if winner 
    // //maybe also territory 
    // new_state->board = board; 
    // new_state->winner = new_state->check_winner();    // check again for a winner
    // new_state->change_turn(); 
    // return new_state;
}

void Cathedral_state::addShapeToBoard(const Cathedral_move *move){ 
     for(int row = 0; row < move->shape.size(); row++){
        for(int col = 0; col < move->shape[row].size(); col++){
            if(board.size() <= row + move->row || board[row].size() <= col + move->col){
                cout << "Shape out of board bounds " << endl;
                continue;
            }
            if(move->shape[row][col] != 0){
                board[row + move->row][col + move->col] = move->shape[row][col]; 
            }
        }
     }
} 

void Cathedral_state::change_turn() {
    turn = (turn == 1) ? 2 : 1;
}


//find a valid move
//also need to track if a piece should be removed 
//this could either be correspond with the action being tried or once a piece is played
//we do a board check to see if any should be removed 

//we also need to track what is a players territory for valid moves 
//if we do a board check after turn we can see if piece should be removed 

//perhaps if territory is created we number it
//

//when to check if territory 
//for actions to try its expensive to search if each action is in territory therefore having number would be efficent 


//when an action is tried we check if territory 

queue<MCTS_move *> *Cathedral_state::actions_to_try() const{
    queue<MCTS_move *> *Q = new queue<MCTS_move *>();

    const std::vector<std::vector<std::vector<int>>>* shapes;
    // cout << "actions to try " << player1Shapes.size() << " " << player2Shapes.size() << endl;
    if (turn == 1) {
        shapes = &player1Shapes;
    } else if (turn == 2) {
        shapes = &player2Shapes;
    } else {
        std::cerr << "Invalid turn number!" << std::endl;
        return Q;
    }
    // cout << "actions to try " << shapes->size() << " "  << endl;

    
    for(int i = 0; i < shapes->size(); i++){
        const auto& shape = (*shapes)[i]; //dereference to access  
        for(int k = 0; k<4; k++){
            for(int x = 0; x < board.size()-shape.size(); x++){
                for(int y = 0; y < board[x].size()-shape.size(); y++){
                    // cout << board[x].size() << " " <<  shape.size() << " " <<  board[x].size()-shape.size() << endl; //printing this 18446708949467004946
                    if(canPlaceShapeAtPos(shape,x,y)){
                        Q->push(new Cathedral_move(x,y,shape, i));
                    }

                }
            }

            rotateMatrix(shape);
        }
    }
    if(Q->empty()){
        cout << "No actions to try"<< endl;
    }
    return Q;
}

bool Cathedral_state::canPlaceShapeAtPos(const vector<vector<int>>& shape, int startRow, int startCol) const{
    for (int i = 0; i < shape.size(); i++) {
        for (int j = 0; j < shape[i].size(); j++) {
            if (shape[i][j] == 0) {
                continue;
            }
            if (board[startRow + i][startCol + j] != 0) {
                return false;
            }
        }
    }

    return true;
}

void Cathedral_state::print() const{
    cout << "Print function ";
}

double Cathedral_state::rollout() const{
    #define MAXSTEPS 50
    #define EVALUATION_THRESHOLD 0.8     // when eval is this skewed then don't simulate any more, return eval


    Cathedral_state curState(*this);
    bool noerror;

    for(int i = 0 ; i < MAXSTEPS; i++){
        if(curState.is_terminal()){
            cout << "Terminal in rollout" << endl;
            return (curState.check_winner() == 1) ? 1.0 : 0.0; //if white win chance return if black delt with in selectbestchild? idk check quoridor 
        }

        //could evaluate position here //or jsut simulate 

        Cathedral_move *m = pick_semirandom_move(curState);
        //if legal move 
        //play move 
        if(!curState.legal_move(m)){
            cout << "Picked illegal move: " << ((m != NULL) ? m->sprint() : "NULL" ) << " intentionally! Move history:" << endl;
        }

        noerror = curState.play_move(m);
        if (!noerror) {
            cerr << "Error: in rollouts" << endl;
            delete m;
            break;
        }
        delete m;
    }
    return evaluate_position(curState);
}


bool Cathedral_state::play_move(const Cathedral_move *move){
    if (move == NULL || !legal_move(move)) {
        cout << "Invalid move in play move" << endl;
        return false;
    }
     for(int i = 0; i < move->shape.size(); i++){
        for(int j = 0; j < move->shape[i].size(); j++){
            if(move->shape[i][j] == 0){
                continue;
            }
            if (move->row + i < board.size() && move->col + j < board[0].size()
               && board[move->row+i][move->col+j] == 0) {            
                
                 if (turn == 1) {
                    if(move->shapeIndex >= player1Shapes.size()){
                        cout << "shapeIndex 1 to big " << move->shapeIndex << " " <<  player1Shapes.size() << endl;
                                        return false;

                    }
                    else{
                         player1Shapes.erase(player1Shapes.begin() + move->shapeIndex);
                    }
                   
                } else if (turn == 2) {
                     if(move->shapeIndex >= player2Shapes.size()){
                        cout << "shapeIndex 2 to big " << move->shapeIndex << " " <<  player2Shapes.size() << endl;
                                        return false;

                    }
                    else{
                        player2Shapes.erase(player2Shapes.begin() + move->shapeIndex);
                    }
                }

                board[move->row+i][move->col+j] = move->shape[i][j];
                winner = check_winner();
                change_turn();
            }
            else {
                cout << "Invalid shape placement cant play move! " << endl;
                return false;
            }
        }
    }
    
    return true;
}

bool Cathedral_state::legal_move(const Cathedral_move *move) {
    if (move == NULL) return false;
    if (turn != 1 && turn != 2){
        cerr << "Warning: wrong player argument!" << endl;
        return false;
    }
    for(int i = 0; i < move->shape.size(); i++){
        for (int j = 0; j < move->shape[i].size(); j++){
            if(move->shape[i][j] == 0){
                continue;
            }
            if(move->row + i >= board.size() || move->col + j >= board[0].size()
               || board[move->row+i][move->col+j] != 0) {            
                return false;
            }
        }
    }
    return true;
}


double Cathedral_state::evaluate_position(Cathedral_state &s) const {
    int player1RemainingShapes = s.player1Shapes.size();
    int player2RemainingShapes = s.player2Shapes.size();
    int shapeDifference = player1RemainingShapes - player2RemainingShapes;

    if (shapeDifference > 0) {
        if(shapeDifference <= 1){
            return 0.7;
        }
        return 1.0;  // Player 1 is winning
    } else if (shapeDifference < 0) {
        if(shapeDifference >= -1){
            return 0.3;
        }
        return 0.0;  // Player 2 is winning
    } else {
        return 0.5;  // Draw
    }
}




    //these comments based on quoridor rollout chatgpt 
    //set threshold and copy state to not mess with cur state 

    //check if state terminal 
    //evaluate position? if in threshold breaks loop 

    //pick semirandommove 
    //check legal 
    //play move 

    //after completing simulation or reaching maxsteps it returns a final position evaluation 



Cathedral_move *Cathedral_state::pick_semirandom_move(Cathedral_state &s) const{
    //i want to keep thsi simple for initial poc 
    
    //future ideas 
    //close to own pieces 
    //somehow blocking enemys from creating territory 
    // 
    const std::vector<std::vector<std::vector<int>>>* shapes;

    if (turn == 1) {
        if (s.player1Shapes.empty()){
            cout << "noPlayer1Shapes" << endl; 
            return nullptr;
        }

        shapes = &s.player1Shapes;

    } else if (turn == 2) {
        if (s.player2Shapes.empty()){
             cout << "noPlayer2Shapes" << endl; 
             return nullptr;
        }

        shapes = &s.player2Shapes;

    } else {
        cout << "Invalid turn number!" << std::endl;
        return nullptr;
    }
        
    for(int i = 0; i < shapes->size(); i++){
        // cout << shapes->size() << " sshapes size " <<endl; 
        const auto& shape = (*shapes)[i]; //dereference to access  
        // for(int k = 0; k<4; k++){
            for(int x = 0; x < s.board.size()-shape.size(); x++){
                for(int y = 0; y < s.board[x].size()-shape[0].size(); y++){
                    // cout << board[x].size() << " " <<  shape.size() << " " <<  board[x].size()-shape.size() << endl; //printing this 18446708949467004946
                    if(s.canPlaceShapeAtPos(shape,x,y)){
                        
                        // cout << "Semi Random move picked " << x << " " << y << " " << "turn " << turn << " " << "" << i << endl;
                        // printMatrix(shape);
                        return new Cathedral_move(x,y,shape, i);
                    }

                }
            }

        //     rotateMatrix(shape);
        // }
    }

    cout << "no random move" <<endl;

}



//generate all moves 

//one step 
//on a blank board any given piece has (64 - 100) positions x 4 rotations
//14 pieces
//so absolute upper limit for one step is 100 x 4 x 14 = 5600



//side note to player mcts combination 
//have the board in center then the pieces in a vector being printed on the sides so as we play a piece the vector gets popped 