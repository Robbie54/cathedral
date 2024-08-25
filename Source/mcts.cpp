
#include "Headers/global.h"
#include "Headers/mcts.h"
#include "Headers/matrix_utility.h"

#include <vector>
#include <iostream>
#include <utility>

using namespace std;

// std::vector<std::vector<std::vector<int>>> shapeFullList{  
//     {{1}}, //Tavern
//     {{1}},
//     {{1, 1}}, //Stable
//     {{1, 1}},
//     {{1, 1},{0,1}}, //Inn
//     {{1,1},{0,1}},
//     {{1,1,1}}, //Bridge
//     {{1, 1},{1,1}}, //Square
//     {{1, 1,1},{0,1,0}}, //Manor
//     {{0, 1,1},{1,1,0}}, //Abbey
//     {{1, 1,0},{0,1,1}, {0,1,0}}, //Academy
//     {{0, 1,0},{1,1,1}, {0,1,0}}, //Infirmary 
//     {{1, 1,1},{1,0,1}}, //Castle
//     {{0, 1,1},{1,1,0},{1,0,0}}}; //Tower

std::vector<std::vector<std::vector<int>>> shapeFullListPOne{  
    {{2}}, //Tavern
    {{3}},
    {{4, 4}}, //Stable
    {{5, 5}},
    {{6, 6},{0,6}}, //Inn
    {{7,7},{0,7}},
    {{8,8,8}}, //Bridge
    {{9, 9},{9,9}}, //Square
    {{10, 10,10},{0,10,0}}, //Manor
    {{0, 11,11},{11,11,0}}, //Abbey
    {{12, 12,0},{0,12,12}, {0,12,0}}, //Academy
    {{0, 13,0},{13,13,13}, {0,13,0}}, //Infirmary 
    {{14, 14,14},{14,0,14}}, //Castle
    {{0, 15,15},{15,15,0},{15,0,0}}}; //Tower

std::vector<std::vector<std::vector<int>>> shapeFullListPTwo{  
    {{26}}, //Tavern
    {{27}},
    {{28, 28}}, //Stable
    {{29, 29}},
    {{30, 30},{0,30}}, //Inn
    {{31,31},{0,31}},
    {{32,32,32}}, //Bridge
    {{33, 33},{33,33}}, //Square
    {{34, 34,34},{0,34,0}}, //Manor
    {{0, 35,35},{35,35,0}}, //Abbey
    {{36, 36,0},{0,36,36}, {0,36,0}}, //Academy
    {{0, 37,0},{37,37,37}, {0,37,0}}, //Infirmary 
    {{38, 38,38},{38,0,38}}, //Castle
    {{0, 39,39},{39,39,0},{39,0,0}}}; //Tower




std::pair<int, int> mcts::calculateScore(const std::vector<std::vector<int>>& matrix){
    int playerOneScore = 0;
    int playerTwoScore = 0;

    for(int i = 0; i < minCol; i++){
        for(int j = 0; j < SCREEN_HEIGHT; j++){
            if(matrix[j][i] >= player1Min && matrix[j][i] <= player1Max){
                playerOneScore++;
            }
        }
    }

    for(int i = maxCol; i<SCREEN_WIDTH ; i++){
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
Cathedral_state::Cathedral_state() : MCTS_state(), board(10, std::vector<int>(10, 0)), winner(0), turn(1), player1Shapes(shapeFullListPOne), player2Shapes(shapeFullListPTwo) {
}

Cathedral_state::Cathedral_state(const Cathedral_state &other) : MCTS_state(other), winner(other.winner), board(other.board), turn(other.turn), player1Shapes(other.player1Shapes), player2Shapes(other.player2Shapes){
   
};

bool Cathedral_state::is_terminal() const {
    int winner = check_winner();
    if(winner == 1 || winner == 2 || winner == 3) {
        return true;
    }
    else {
        return false;
    }
}

int Cathedral_state::check_winner() const {

    
    queue<MCTS_move*> *Q = actions_to_try();
    if(Q->empty()){
        delete Q;
        // cout << "que empty in check winner " << endl;
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

int Cathedral_state::change_turn() {
    turn = (turn == 1) ? 2 : 1;
    return turn;
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

    //starts with big pieces first
    for(int i = shapes->size()-1; i >= 0; i--){
        const auto& shape = (*shapes)[i]; //dereference to access  
        for(int k = 0; k<4; k++){
            for(int x = 0; x < board.size()-shape.size(); x++){
                for(int y = 0; y < board[x].size()-shape.size(); y++){
                    // cout << board[x].size() << " " <<  shape.size() << " " <<  board[x].size()-shape.size() << endl; //printing this 18446708949467004946
                    if(canPlaceShapeAtPos(shape,x,y)){ //!TODO && not in territory 
                        Q->push(new Cathedral_move(x,y,shape));
                    }

                }
            }

            rotateMatrix(shape);
        }
    }
    if(Q->empty()){
        // cout << "No actions to try"<< endl;
    }
    cout << "Actions to try size " << Q->size() << endl;
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
    for (const auto& row : board) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
    cout << "Print function ";
}

double Cathedral_state::rollout() const{
    #define MAXSTEPS 50
    #define EVALUATION_THRESHOLD 0.8     // when eval is this skewed then don't simulate any more, return eval


    Cathedral_state curState(*this);
    bool noerror;

    for(int i = 0 ; i < MAXSTEPS; i++){
        if(curState.is_terminal()){
            // cout << "Terminal in rollout" << endl;
            return (curState.check_winner() == 1) ? 1.0 : 0.0; //if white win chance return if black delt with in selectbestchild? idk check quoridor 
        }

        //could evaluate position here //or jsut simulate 

        Cathedral_move *m = curState.pick_semirandom_move(curState);
        //if legal move 
        //play move 
        if(!curState.legal_move(m)){
            cout << "Picked illegal move intentionally! Move history:" << endl;
            return evaluate_position(curState);
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

//play move might be deleting to many shapes per move
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

                board[move->row+i][move->col+j] = move->shape[i][j];
                
            }
            else {
                cout << "Invalid shape placement cant play move! " << endl;
                return false;
            }
        }
    }

    int target;
    for (const auto& inner : move->shape) {
        for (int num : inner) {
            if (num != 0) {
                target = num; // Return the first non-zero integer found
            }
        }
    }

    if(turn == 1){
        int x = containsInt(player1Shapes, target);
        player1Shapes.erase(player1Shapes.begin() + x);

    }
    else if(turn == 2){
        int x = containsInt(player2Shapes, target);
        player2Shapes.erase(player2Shapes.begin() + x);
    }
    else {
        cout << "Error with turn in play move " << endl;
    }

        winner = check_winner();
        change_turn();
        
    return true;
}

int Cathedral_state::containsInt(const std::vector<std::vector<std::vector<int>>>& vec, int target) {
    for (int i = 0; i < vec.size(); ++i) {
        for (const auto& inner : vec[i]) {
            if (!inner.empty() && inner[0] != 0 && inner[0] != target) {
                break; // Move to the next outer vector if a different non-zero value is found
            }
            if (std::find(inner.begin(), inner.end(), target) != inner.end()) {
                return i;
            }
        }
    }

    cout << "Error no match with shape placed and list in containsInt returning 0 target is " << target << endl;
    return 0;
}


bool Cathedral_state::legal_move(const Cathedral_move *move) {
    if (move == NULL) return false;
    if(move->row > board.size() || move->row < 0 || move->col > board[0].size() || move->col < 0){
        cout << "Not legal move row or coloumn out of range proberly undefined " << endl;
        return false;
    }
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

    // if (turn == 1) {
    //     if (s.player1Shapes.empty()){
    //         cout << "noPlayer1Shapes" << endl; 
    //         return nullptr;
    //     }

    //     shapes = &s.player1Shapes;

    // } else if (turn == 2) {
    //     if (s.player2Shapes.empty()){
    //          cout << "noPlayer2Shapes" << endl; 
    //          return nullptr;
    //     }

    //     shapes = &s.player2Shapes;

    // } else {
    //     cout << "Invalid turn number!" << std::endl;
    //     return nullptr;
    // }


    queue<MCTS_move*> *Q = s.actions_to_try();

    if(!Q->empty()){
        MCTS_move *m = Q->front();
        return static_cast<Cathedral_move*>(m);
    }



    cout << "NO RANDOM MOVE IN PICK SEMIRANDOM MOVE" <<endl;

}



//generate all moves 

//one step 
//on a blank board any given piece has (64 - 100) positions x 4 rotations
//14 pieces
//so absolute upper limit for one step is 100 x 4 x 14 = 5600



//side note to player mcts combination 
//have the board in center then the pieces in a vector being printed on the sides so as we play a piece the vector gets popped 




//check if out of area doesn't owrk 
vector<vector<int>> Cathedral_state::updatePieces(int player) {

    int currentRow = 0, currentCol = 0;
    int maxHeightInRow = 0; // To keep track of the tallest shape in the current row
   
    vector<vector<vector<int>>> shapes;

    vector<vector<int>> resultMatrix((PIECE_SPACE_WIDTH), vector<int>(PIECE_SPACE_HEIGHT,0));

    if(player == 1){
        shapes = player1Shapes;
    }
    else if(player == 2) {
        shapes = player2Shapes;
    }
    else{
        cout << "Invalid player in update pieces " << endl;
    }

    for (const auto& shape : shapes) {
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        
        // If the shape does not fit horizontally, move to the next row
        if (currentCol + shapeWidth > PIECE_SPACE_WIDTH) {
            currentRow += maxHeightInRow + 1; // Move to the next row with a gap
            currentCol = 0; // Reset the column position
            maxHeightInRow = 0; // Reset the max height for the new row
        }

        // If the shape does not fit vertically, stop placing shapes
        if (currentRow + shapeHeight > PIECE_SPACE_HEIGHT) {
            cout << "Shapes cannot fit within the given dimensions." << endl;
            return resultMatrix;
        }

        // Place the shape in the result matrix
        for (int i = 0; i < shapeHeight; ++i) {
            for (int j = 0; j < shapeWidth; ++j) {
                resultMatrix[currentRow + i][currentCol + j] = shape[i][j];
            }
        }

        // Update current column and max height in the current row
        currentCol += shapeWidth + 1; // Move to the next position with a gap
        maxHeightInRow = max(maxHeightInRow, shapeHeight); // Update the max height if needed
    }

    return resultMatrix;
}