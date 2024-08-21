#pragma once

//this class is for calculations that happen on the board 
//possibly add an update board on map in matrix utility class 

//will need mousePos, board, current shape being placed 
class board_utility{
    public:
    board_utility(const int& currentPlayer, const std::vector<std::vector<int>>& board)
            : player(currentPlayer), _board(board), rows(board.size()), cols(board[0].size()) {
            notPlayer = (player == 1) ? 2 : 1;
            if (player == 1) {
                _playerMin = player1Min;
                _playerMax = player1Max;
                _opponentMin = player2Min;
                _opponentMax = player2Max;
            } else {
                _playerMin = player2Min;
                _playerMax = player2Max;
                _opponentMin = player1Min;
                _opponentMax = player1Max ;
            }
        }   
    

    // checkTeritory
    bool checkNotOpponentsTeritory(std::vector<std::vector<int>> matrix, const sf::Vector2f& mousePosWorld);
    
    //to remove one piece
    bool checkIfCreatingTerritory(std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& shape, const sf::Vector2f& mousePosWorld);

    private:
        std::vector<std::vector<int>> _board;

        const int& player;
        int notPlayer;
        int _playerMin; 
        int _playerMax; 
        int _opponentMin; 
        int _opponentMax; 

        int rows = _board.size();
        int cols = _board[0].size();

        std::vector<std::pair<int,int>> piecePosToRemove;
        int pieceNum;

        std::vector<std::pair<int, int>> positionsAroundShape(const std::vector<std::vector<int>>& shape);

        //checking placement not in enemy territory 
        bool checkPosition(int x, int y,std::vector<std::vector<bool>>& visited); //recursive function 

        //check position is not one enemy piece in territory
        bool checkPositionForPieceRemoval(int x, int y, std::vector<std::vector<bool>>& visited, int& pieceInside);

        void addShapeBack(std::vector<std::vector<int>>& map);


        

        std::vector<std::vector<int>> startingMap = {
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


};