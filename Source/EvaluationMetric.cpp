#include "Headers/EvaluationMetric.h"
#include "Headers/Global.h"
#include "Headers/MatrixUtility.h"
#include <vector>
#include <iostream>

double EvaluationMetric::evaluate(const Cathedral_state& state) {
    // Safety check to ensure state is valid
    try {
        const auto& board = state.get_state_info().board;
        if (board.empty() || board[0].empty()) {
            std::cerr << "Error: Invalid board state in evaluation" << std::endl;
            return 0.0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing state info: " << e.what() << std::endl;
        return 0.0;
    }

    int freeSpaces = countFreeSpaces(state);
    int playerTerritory = countPlayerTerritory(state);
    int enemyTerritory = countEnemyTerritory(state);
    int playerPieces = countPlayerPieces(state);
    int enemyPieces = countEnemyPieces(state);
    auto unplayablePlayerResult = countUnplayablePiecesForPlayer(state);

    int unplayablePlayerPieces = unplayablePlayerResult.count;
    int unplayablePlayerSquares = unplayablePlayerResult.squares;

    // Better weight balance
    const double wTerritory = 1.5;
    const double wPieces = 0.5;
    const double wFreeSpaces = 0.0;
    const double wUnplayablePieces = -1.2;
    const double wUnplayableSquares = -0.05;

    const double boardArea = BOARD_SIZE * BOARD_SIZE;
    const double maxPieces = static_cast<double>(shapeFullListPOne.size());

    // Normalize
    double normTerritoryDiff = (playerTerritory - enemyTerritory) / boardArea;

    int pieceOffset = state.player1_turn() ? 4 : 0; //update late to minus the squares p1 played last
    double normPiecesDiff = (playerPieces - enemyPieces) / maxPieces;
    double normFreeSpaces = freeSpaces / boardArea;
    double normUnplayablePieces = unplayablePlayerPieces / maxPieces;
    double normUnplayableSquares = unplayablePlayerSquares / boardArea;

    // Score calculation
    double score =
        wTerritory * normTerritoryDiff +
        wPieces * normPiecesDiff +
        wUnplayablePieces * normUnplayablePieces;

    bool isPlayer1 = state.player1_turn();
    std::string playerColor = isPlayer1 ? "Green" : "Red";
    std::string enemyColor = isPlayer1 ? "Red" : "Green";

    std::cout << playerColor <<"'s chance is " << score << std::endl;

    return std::clamp(score, -1.0, 1.0);
}




    

    // std::cout << "Free spaces: " << freeSpaces << std::endl;
    // std::cout << playerColor << " territory spaces: " << playerTerritory << std::endl;
    // std::cout << enemyColor << " territory spaces: " << enemyTerritory << std::endl;
    // std::cout << playerColor << " pieces on board: " << playerPieces << std::endl;
    // std::cout << enemyColor << " pieces on board: " << enemyPieces << std::endl;
    // std::cout << playerColor << " unable to play pieces: " << unplayablePlayerPieces << std::endl;
    // std::cout << playerColor << " unable to play squares: " << unplayablePlayerSquares << std::endl;
    // std::cout << playerColor << " unable to play piece indices: ";
    // for (size_t i = 0; i < unplayablePlayerPieceIndices.size(); ++i) {
    //     std::cout << unplayablePlayerPieceIndices[i];
    //     if (i != unplayablePlayerPieceIndices.size() - 1) std::cout << ", ";
    // }
    // std::cout << std::endl;
    // return -1; // Placeholder score

int EvaluationMetric::countFreeSpaces(const Cathedral_state& state) {
    return countMatrixIndex(state.get_state_info().board, 0);
}

int EvaluationMetric::countPlayerTerritory(const Cathedral_state& state) {
    int playerID = state.player1_turn() ? player1Territory : player2Territory;
    return countMatrixIndex(state.get_state_info().board, playerID);
}

int EvaluationMetric::countEnemyTerritory(const Cathedral_state& state) {
    int enemyID = state.player1_turn() ? player2Territory : player1Territory;
    return countMatrixIndex(state.get_state_info().board, enemyID);
}

int EvaluationMetric::countPlayerPieces(const Cathedral_state& state) {
    int minVal = state.player1_turn() ? player1Min : player2Min;
    int maxVal = state.player1_turn() ? player1Max : player2Max;
    return countMatrixRange(state.get_state_info().board, minVal, maxVal);
}

int EvaluationMetric::countEnemyPieces(const Cathedral_state& state) {
    int minVal = state.player1_turn() ? player2Min : player1Min;
    int maxVal = state.player1_turn() ? player2Max : player1Max;
    return countMatrixRange(state.get_state_info().board, minVal, maxVal);
}

bool EvaluationMetric::checkCathedralOnBoard(const Cathedral_state& state) {
    int count = countMatrixIndex(state.get_state_info().board, cathedral);
    if(count == 0){
        return false;
    }
    
    return true;
}


UnplayableInfo EvaluationMetric::countUnplayablePiecesForPlayer(const Cathedral_state& state) {
    int unplayablePieces = 0;
    int unplayableSquares = 0;
    std::vector<int> unplayableIndices;
    const auto& board = state.get_state_info().board;
    const auto& playerShapes = state.player1_turn() ? state.get_state_info().player1Shapes
                                                    : state.get_state_info().player2Shapes;

    for (size_t i = 0; i < playerShapes.size(); ++i) {
        bool canPlace = false;
        std::vector<std::vector<int>> currentShape = playerShapes[i];

        for (int rot = 0; rot < 4; ++rot) {
            int shapeRows = currentShape.size();
            int shapeCols = shapeRows > 0 ? currentShape[0].size() : 0;
            for (size_t x = 0; x + shapeRows <= board.size(); ++x) {
                for (size_t y = 0; y + shapeCols <= board[0].size(); ++y) {
                    Cathedral_move move(x, y, currentShape);
                    if (state.legal_move(&move)) {
                        canPlace = true;
                        break;
                    }
                }
                if (canPlace) break;
            }
            if (canPlace) break;
            currentShape = rotateMatrix(currentShape);
        }
        if (!canPlace) {
            unplayablePieces++;
            unplayableIndices.push_back(i);
            for (const auto& row : playerShapes[i]) {
                for (int val : row) {
                    if (val != 0) unplayableSquares++;
                }
            }
        }
    }
    return {unplayablePieces, unplayableIndices, unplayableSquares};
}



int EvaluationMetric::countMatrixIndex(const std::vector<std::vector<int>>& board, int index) {
    return countMatrixRange(board, index, index);
}

int EvaluationMetric::countMatrixRange(const std::vector<std::vector<int>>& board, int minIndex, int maxIndex) {
    int count = 0;
    for (const auto& row : board) {
        for (int val : row) {
            if (val >= minIndex && val <= maxIndex)
                count++;
        }
    }
    return count;
}
