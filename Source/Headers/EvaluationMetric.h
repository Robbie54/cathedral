#pragma once

#include "../../MonteCarloTreeSearch-main/mcts/include/state.h"
#include "CathedralState.h"
#include <vector>
#include <map>
#include <string>

struct UnplayableInfo {
    int count;
    std::vector<int> indices;
    int squares;
};

// Evaluation component results
struct EvaluationComponent {
    double rawValue;
    double normalizedValue;
    double weightedValue;
    std::string description;
};

// Game phase enum for dynamic evaluation
enum class GamePhase {
    EARLY,      // Cathedral placement + first few moves (0-20% pieces placed)
    MIDDLE,     // Territory claiming and positioning (20-70% pieces placed)
    LATE        // Piece placement focus (70%+ pieces placed)
};

// Evaluation breakdown for debugging and tuning
struct EvaluationBreakdown {
    std::map<std::string, EvaluationComponent> components;
    double totalScore;
    GamePhase gamePhase;
    std::string analysis;
    int unplacedSquaresPlayer;
    int unplacedSquaresOpponent;
};

class EvaluationMetric {
public:
    // Main evaluation functions
    static double evaluate(const Cathedral_state& state);
    static EvaluationBreakdown evaluateDetailed(const Cathedral_state& state);
    
    // Core evaluation components (rule-based)
    static double evaluateWinCondition(const Cathedral_state& state);        // Primary: unplaced squares
    static double evaluateTerritoryControl(const Cathedral_state& state);    // Secondary: claimed territory
    static double evaluatePieceMobility(const Cathedral_state& state);       // Piece placement options
    static double evaluatePositionalAdvantage(const Cathedral_state& state); // Board position quality
    static double evaluateThreatAssessment(const Cathedral_state& state);    // Capture threats
    static double evaluateCathedralStrategy(const Cathedral_state& state);   // Cathedral positioning
    
    // Game analysis helpers
    static GamePhase determineGamePhase(const Cathedral_state& state);
    static double getPhaseWeight(const std::string& component, GamePhase phase);
    
    // Legacy functions (maintained for compatibility)
    static int countFreeSpaces(const Cathedral_state& state);
    static int countPlayerTerritory(const Cathedral_state& state);
    static int countEnemyTerritory(const Cathedral_state& state);
    static int countPlayerPieces(const Cathedral_state& state);
    static int countEnemyPieces(const Cathedral_state& state);
    static bool checkCathedralOnBoard(const Cathedral_state& state);
    static UnplayableInfo countUnplayablePiecesForPlayer(const Cathedral_state& state);
    
private:
    // Advanced analysis functions
    static int calculateUnplacedSquares(const Cathedral_state& state, bool forCurrentPlayer);
    static std::vector<std::pair<int, int>> findEnclosedRegions(const Cathedral_state& state);
    static double assessPieceConnectivity(const Cathedral_state& state);
    static double calculateTerritoryPotential(const Cathedral_state& state);
    static double evaluateDefensivePositioning(const Cathedral_state& state);
    static double calculateMobilityScore(const Cathedral_state& state);
    static double assessCaptureThreats(const Cathedral_state& state);
    
    // Utility functions
    static int countMatrixIndex(const std::vector<std::vector<int>>& board, int index);
    static int countMatrixRange(const std::vector<std::vector<int>>& board, int minIndex, int maxIndex);
    static double normalizeScore(double raw, double min, double max);
    
    // Weight configuration for different game phases
    static const std::map<std::string, std::map<GamePhase, double>> PHASE_WEIGHTS;
    static void initializeWeights();
};
