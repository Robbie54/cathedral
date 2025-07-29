
#include "Headers/Global.h"
#include "Headers/CathedralState.h"
#include "Headers/MatrixUtility.h"
#include "Headers/BoardUtility.h"

#include <vector>
#include <iostream>
#include <utility>
#include <climits>
#include <random>

default_random_engine Cathedral_state::generator = default_random_engine(time(NULL));


using namespace std;




//in ttt this is called once before do while in quoridor same only for new game
//no winner is calcualted 
Cathedral_state::Cathedral_state() : MCTS_state(), board(10, std::vector<int>(10, 0)), winner(0), turn(1), player1Shapes(shapeFullListPOne), player2Shapes(shapeFullListPTwo), playerMin(player1Min), playerMax(player1Max), opponentMin(player2Min), opponentMax(player2Max), playerTerritory(player1Territory), opponentTerritory(player2Territory){
}

Cathedral_state::Cathedral_state(const Cathedral_state &other) : MCTS_state(other), winner(other.winner), board(other.board), turn(other.turn), player1Shapes(other.player1Shapes), player2Shapes(other.player2Shapes), playerMin(other.playerMin), playerMax(other.playerMax), opponentMin(other.opponentMin), opponentMax(other.opponentMax), playerTerritory(other.playerTerritory), opponentTerritory(other.opponentTerritory) {
   
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

    bool a = any_actions_to_try();

    if(a == false){
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

// int Cathedral_state::check_winner() const {

    
//     queue<MCTS_move*> *Q = actions_to_try();
//     if(Q->empty()){
//         delete Q;
//         // cout << "que empty in check winner " << endl;
//         if(player1Shapes.size() > player2Shapes.size()){
//             return 2;
//         }
//         else if(player2Shapes.size() > player1Shapes.size()){
//             return 1;
//         }
//         else{
//             return 3; //tie
//         }
//     }

//     //   Clean up dynamically allocated MCTS_move* objects in the queue  //this helped significantly with memeory i think actions to try is causing the main memory leaks 
//     while (!Q->empty()) {
//         MCTS_move* move = Q->front();
//         Q->pop();
//         delete move;
//     }

//     delete Q;
//     return 0;
// }

bool Cathedral_state::any_actions_to_try() const{

    std::vector<std::vector<std::vector<int>>> shapes;
    // cout << "actions to try " << player1Shapes.size() << " " << player2Shapes.size() << endl;
    if (turn == 1) {
        shapes = player1Shapes;
    } else if (turn == 2) {
        shapes = player2Shapes;
    } else {
        std::cerr << "Invalid turn number!" << std::endl;
        return false;
    }
    

    //this is not ideal as there is actions to try but idk
     int y = 0;
    if(shapes.size() > 4){
        y = shapes.size()-3;
    }

    for (int i = shapes.size() - 1; i >= 0; i--) {
        std::vector<std::vector<int>> currentShape = shapes[i];  // Extract 2D shape
        for (int k = 0; k < 4; k++) {  // 4 rotations
            for (int x = 0; x <= board.size() - currentShape.size(); x++) {
                for (int y = 0; y <= board[x].size() - currentShape[0].size(); y++) {
                    Cathedral_move move(x, y, currentShape);

                    if (legal_move(&move)) {
                       return true;
                    }
                }
            }

             int shapeNum = 0; // Default value

            if (!currentShape.empty() && !currentShape[0].empty()) {
                shapeNum = currentShape[0][0] != 0 ? currentShape[0][0] : (currentShape[0].size() > 1 ? currentShape[0][1] : 0);
            }
            if(shapeNum ==13 ||shapeNum == 37 || shapeNum == 33 || shapeNum ==9){
                k=4;
                continue;
            }
            // Rotate the 2D shape and reassign it
            currentShape = rotateMatrix(currentShape);
        }

        shapes[i] = currentShape;  // Assign rotated shape back to 3D vector
    }
    
    // cout << "Actions to try size " << Q->size() << endl;
    return false;
}






MCTS_state *Cathedral_state::next_state(const MCTS_move *move) const {
    Cathedral_state *new_state = new Cathedral_state(*this);
    new_state->play_move((const Cathedral_move *) move);
    return new_state;
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
    std::swap(playerMin, opponentMin);
    std::swap(playerMax, opponentMax);
    std::swap(playerTerritory, opponentTerritory);
    turn = (turn == 1) ? 2 : 1;
    return turn;
}


queue<MCTS_move *> *Cathedral_state::actions_to_try() const{
    queue<MCTS_move *> *Q = new queue<MCTS_move *>();

    std::vector<std::vector<std::vector<int>>> shapes;
    // cout << "actions to try " << player1Shapes.size() << " " << player2Shapes.size() << endl;
    if (turn == 1) {
        shapes = player1Shapes;
    } else if (turn == 2) {
        shapes = player2Shapes;
    } else {
        std::cerr << "Invalid turn number!" << std::endl;
        return Q;
    }
    // cout << "actions to try " << shapes->size() << " "  << endl;

    //starts with big pieces first
    int p = 0;
    if(shapes.size() > 4){
        p = shapes.size()-3;
    }

    for (int i = shapes.size() - 1; i >= 0; i--) {
        std::vector<std::vector<int>> currentShape = shapes[i];  // Extract 2D shape
        for (int k = 0; k < 4; k++) {  // 4 rotations
            for (int x = 0; x <= board.size() - currentShape.size(); x++) {
                for (int y = 0; y <= board[x].size() - currentShape[0].size(); y++) {
                    Cathedral_move move(x, y, currentShape);

                    if (legal_move(&move)) {
                        Q->push(new Cathedral_move(x, y, currentShape));
                    }
                }
            }
            
            int shapeNum = 0; // Default value

            if (!currentShape.empty() && !currentShape[0].empty()) {
                shapeNum = currentShape[0][0] != 0 ? currentShape[0][0] : (currentShape[0].size() > 1 ? currentShape[0][1] : 0);
            }
            if(shapeNum ==13 ||shapeNum == 37 || shapeNum == 33 || shapeNum ==9){
                k=4;
                continue;
            }
            // Rotate the 2D shape and reassign it
            currentShape = rotateMatrix(currentShape);
        }

        shapes[i] = currentShape;  // Assign rotated shape back to 3D vector
    }
    if(Q->empty()){
        // cout << "No actions to try"<< endl;
    }
    // cout << "Actions to try size " << Q->size() << endl;
    return Q;
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
    #define MAXSTEPS 26 //more then most games atm 
    #define EVALUATION_THRESHOLD 0.8     // when eval is this skewed then don't simulate any more, return eval


    Cathedral_state curState(*this);
    bool noerror;

    for(int i = 0 ; i < MAXSTEPS; i++){
        if(curState.is_terminal()){
            //  cout << "Terminal in rollout" << endl;
            // printMatrix(curState.board);
            // cout << curState.player1Shapes.size() << " " << curState.player2Shapes.size() << endl;
            return (curState.check_winner() == 1) ? 1.0 : 0.0; //if white win chance return if black delt with in selectbestchild? idk check quoridor 
            
        }

        Cathedral_move *m = curState.pickRandomMove(curState); //random games

        // Cathedral_move *m = curState.pick_semirandom_move(curState);
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
               && board[move->row+i][move->col+j] == 0 || board[move->row+i][move->col+j] == playerTerritory) { 
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
        // Use BoardUtility for territory checking
        BoardUtility boardUtil(turn , board);
        bool territoryCreated = false;
        if (player1Shapes.size() < shapeFullListPOne.size() - 1 || player2Shapes.size() < shapeFullListPTwo.size() - 1){
            boardUtil.setTerritoryInfo(playerTerritory, opponentTerritory);
            territoryCreated = boardUtil.checkIfCreatingTerritory(move);
        }
        
        if (territoryCreated) {
            // Get the updated board from BoardUtility
            board = boardUtil.getBoard();
            
            // Handle piece removal if needed
            int pieceToRemove = boardUtil.getPieceNumToRemove();
            if (pieceToRemove != -1 && pieceToRemove != 1) {
                addShapeToPlayerShapes(pieceToRemove);
            }
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


bool Cathedral_state::legal_move(const Cathedral_move *move) const{
    if (move == NULL) {
        cout <<"NULL Move in legal Move " << endl; 
        return false;
    }

    if(move->row >= board.size() || move->row < 0 || move->col >= board[0].size() || move->col < 0){
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
               || (board[move->row+i][move->col+j] != 0 && board[move->row + i] [move->col + j] != playerTerritory)){      //&& board[move->row + i] [move->col + j] != playerTerritory
                return false;
            }
        }
    }

    return true;
}


//currently rarely used because usually rollout is terminal before max steps reachs
double Cathedral_state::evaluate_position(Cathedral_state &s) const {
    int player1RemainingShapes = s.player1Shapes.size();
    int player2RemainingShapes = s.player2Shapes.size();
    int shapeDifference = player1RemainingShapes - player2RemainingShapes;
    if (shapeDifference > 0) {
        if(shapeDifference <= 1){
            return 0.7;
        }
        // cout << "player 1 is winning " << endl;
        return 0.9;  // Player 1 is winning
    } else if (shapeDifference < 0) {
        if(shapeDifference >= -1){
            return 0.3;
        }
        // cout << "player 2 is winning " << endl;

        return 0.;  // Player 2 is winning
    } else {
        return 0.5;  // Draw
        cout << "draw " << endl; 
    }
} 

//why does this return a pointer move
Cathedral_move *Cathedral_state::pickRandomMove(Cathedral_state &s) const {
    Cathedral_move *m;
    

    std::vector<std::vector<std::vector<int>>> shapes;
    if (turn == 1) {
        shapes = player1Shapes;
    } else if (turn == 2) {
        shapes = player2Shapes;
    } else {
        std::cerr << "Invalid turn number!" << std::endl;
        return m;
    }

    if(shapes.empty()){
        std::cout << "shapes empty in pick random move";
        return m;
    }
    
    for(int i = 0; i < 500; i ++){ //maybe need to check actions to try after a bit 
    //50 still made more simulations then 5 and then all actions to try after 50 also never/very rare called the all actions to try
    //mostly 0 typically less then 10 occasionally higher 
        std::uniform_int_distribution<> dis(0, shapes.size()-1);
        int shapeIndex = dis(generator);  

        std::uniform_int_distribution<> dis2(1, 4);
        int rotation = dis2(generator); 

        //pick random shape and rotation and position 
        //for random positions maybe a vector of options and pop off when not valid 
        std::vector<std::vector<int>> shapePicked = shapes[shapeIndex];

        for(int i = 0; i < rotation; i++){
            if(shapePicked[0][0] != 13){
                shapePicked = rotateMatrix(shapePicked);
            }
        }

        
        //shuffel an array once of 0 - 9 then iterate through it 
        vector<int> shuffledX(board.size() - shapePicked.size() + 1);
        iota(shuffledX.begin(),shuffledX.end(), 0);
        vector<int> shuffledY(board.size() - shapePicked[0].size() + 1);
        iota(shuffledY.begin(),shuffledY.end(), 0);
        
        for (int x = 0; x <= board.size() - shapePicked.size(); x++) {
                for (int y = 0; y <= board[x].size() - shapePicked[0].size(); y++) {
                    Cathedral_move move(x, y, shapePicked);
                    if(legal_move(&move)) {
                        // if(i > 100){
                        //      cout << "Attempts in random move " << i << endl;
                        // }
                        return new Cathedral_move(x,y,shapePicked);
                    }
            }
        }
    }


    std::cout << "random move selection failed! " << endl;
    return m; 

    //if random move selection fails get all actions to try 
    //this might not be necassary as selecting a 1x1 will almost guarentee finding a move

    // queue<MCTS_move*> *Q = s.actions_to_try();

    // MCTS_move *m2;

    // if(!Q->empty()){
    //      int randomNumber;
    //     if(Q->size()-1 > 0){
    //         std::uniform_int_distribution<> dis(1, Q->size()-1);
    //         randomNumber = dis(generator);  
        
    //     } else {
    //         randomNumber = 1;
    //     }
           
       
    //     for (int i = 0; i < randomNumber; i++) {
    //         m2 = Q->front();
    //         Q->pop();
    //     }

    //     while (!Q->empty()) {
    //         MCTS_move* m2 = Q->front();
    //         Q->pop();
    //         delete m2;  
    //     }
    //     delete Q;  

    //     return static_cast<Cathedral_move*>(m2);
    // }

    // delete Q; 
    // cout << "NO RANDOM MOVE IN PICK RANDOM MOVE" <<endl;
    

}



Cathedral_move *Cathedral_state::pick_semirandom_move(Cathedral_state &s) const{

    queue<MCTS_move*> *Q = s.actions_to_try();

    MCTS_move *m;
    if(!Q->empty()){
        if(Q->size() > 100){
            std::uniform_int_distribution<> dis(1, 100); // Range [1, 100]
            int randomNumber = dis(generator);
            for (int i = 0; i <= randomNumber; ++i) {
                m = static_cast<Cathedral_move*>(Q->front());
                Q->pop();
            }
        }
        else{
            std::uniform_int_distribution<> dis(1, Q->size()-1);
            int randomNumber = dis(generator);

            for (int i = 0; i <= randomNumber; ++i) {
                m = static_cast<Cathedral_move*>(Q->front());
                Q->pop();
            }

        }
        while (!Q->empty()) {
        MCTS_move* m = Q->front();
        Q->pop();
        delete m;  // Clean up individual elements
        }
        delete Q;  // Clean up the queue itself

        return static_cast<Cathedral_move*>(m);
    }

    cout << "NO RANDOM MOVE IN PICK SEMIRANDOM MOVE" <<endl;
    return nullptr;
}


void Cathedral_state::addShapeToPlayerShapes(int pieceNum){
    int i = 0;
    if(turn == 2){
        for (const auto& shape : shapeFullListPOne) {
            if(shape.empty()){
                return;
            }
            if(shape[0][0] == pieceNum || shape[0][1] == pieceNum){
                if(player1Shapes.size() >= i){
                    player1Shapes.insert(player1Shapes.begin() + i, shape);
                }else {
                    player1Shapes.push_back(shape);
                }
            
                return; 
            }
            i++;
        }
    }
    else if(turn == 1){
          for (const auto& shape : shapeFullListPTwo) {
            if(shape.empty()){
                return;
            }
            if(shape[0][0] == pieceNum || shape[0][1] == pieceNum){
                if(player2Shapes.size() >= i){
                    player2Shapes.insert(player2Shapes.begin() + i, shape);
                }else {
                    player2Shapes.push_back(shape);
                }
            
                return; 
            }
            i++;
        }
    }

     cout << "Invalid pieceNum or turn when trying to add shape to player shapes" << endl;
     return;
}



void Cathedral_state::addMove(Cathedral_move m){
    movesVec.push_back(m);
}
    