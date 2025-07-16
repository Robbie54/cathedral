#pragma once

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"
#include "Mcts.h"

struct UnplayableInfo {
    int count;
    std::vector<int> indices;
    int squares;
};

class EvaluationMetric {
public:
    // Basic static evaluation: higher is better for current player
    static double evaluate(const Cathedral_state& state);

private:
    static int countFreeSpaces(const Cathedral_state& state);
    static int countPlayerTerritory(const Cathedral_state& state);  //not sure if this would be better as player or green and red
    static int countEnemyTerritory(const Cathedral_state& state); 
    static int countPlayerPieces(const Cathedral_state& state); 
    static int countEnemyPieces(const Cathedral_state& state); 

    static bool checkCathedralOnBoard(const Cathedral_state& state); 

    static UnplayableInfo countUnplayablePiecesForPlayer(const Cathedral_state& state);

    static int countMatrixIndex(const std::vector<std::vector<int>>& board, int index);
    static int countMatrixRange(const std::vector<std::vector<int>>& board, int minIndex, int maxIndex);


};
