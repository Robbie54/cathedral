#include "Headers/EvaluationMetric.h"
#include "Headers/Global.h"
#include "Headers/MatrixUtility.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

// Weight configuration for different game phases
const std::map<std::string, std::map<GamePhase, double>> EvaluationMetric::PHASE_WEIGHTS = {
    {"win_condition", {{GamePhase::EARLY, 0.3}, {GamePhase::MIDDLE, 0.6}, {GamePhase::LATE, 1.0}}},
    {"territory_control", {{GamePhase::EARLY, 0.4}, {GamePhase::MIDDLE, 0.8}, {GamePhase::LATE, 0.9}}},
    {"piece_mobility", {{GamePhase::EARLY, 0.8}, {GamePhase::MIDDLE, 0.6}, {GamePhase::LATE, 0.5}}},
    {"positional_advantage", {{GamePhase::EARLY, 0.6}, {GamePhase::MIDDLE, 0.7}, {GamePhase::LATE, 0.4}}},
    {"threat_assessment", {{GamePhase::EARLY, 0.2}, {GamePhase::MIDDLE, 0.5}, {GamePhase::LATE, 0.7}}},
    {"cathedral_strategy", {{GamePhase::EARLY, 0.9}, {GamePhase::MIDDLE, 0.4}, {GamePhase::LATE, 0.1}}}
};

double EvaluationMetric::evaluate(const Cathedral_state& state) {
    // Safety check
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
    
    GamePhase phase = determineGamePhase(state);
    
    // Core evaluation components
    double winScore = evaluateWinCondition(state) * getPhaseWeight("win_condition", phase);
    double territoryScore = evaluateTerritoryControl(state) * getPhaseWeight("territory_control", phase);
    double mobilityScore = evaluatePieceMobility(state) * getPhaseWeight("piece_mobility", phase);
    double positionalScore = evaluatePositionalAdvantage(state) * getPhaseWeight("positional_advantage", phase);
    double threatScore = evaluateThreatAssessment(state) * getPhaseWeight("threat_assessment", phase);
    double cathedralScore = evaluateCathedralStrategy(state) * getPhaseWeight("cathedral_strategy", phase);
    
    double totalScore = winScore + territoryScore + mobilityScore + positionalScore + threatScore + cathedralScore;
    
    // Debug output
    bool isPlayer1 = state.player1_turn();
    std::string playerColor = isPlayer1 ? "Green" : "Red";
    std::cout << playerColor << "'s evaluation: " << totalScore 
              << " (Win:" << winScore << " Territory:" << territoryScore 
              << " Mobility:" << mobilityScore << ")" << std::endl;
    
    return totalScore;
}

EvaluationBreakdown EvaluationMetric::evaluateDetailed(const Cathedral_state& state) {
    EvaluationBreakdown breakdown;
    breakdown.gamePhase = determineGamePhase(state);
    
    // Calculate each component
    std::vector<std::pair<std::string, double>> components = {
        {"win_condition", evaluateWinCondition(state)},
        {"territory_control", evaluateTerritoryControl(state)},
        {"piece_mobility", evaluatePieceMobility(state)},
        {"positional_advantage", evaluatePositionalAdvantage(state)},
        {"threat_assessment", evaluateThreatAssessment(state)},
        {"cathedral_strategy", evaluateCathedralStrategy(state)}
    };
    
    breakdown.totalScore = 0.0;
    for (const auto& [name, raw] : components) {
        EvaluationComponent comp;
        comp.rawValue = raw;
        comp.normalizedValue = raw; // Already normalized in component functions
        comp.weightedValue = raw * getPhaseWeight(name, breakdown.gamePhase);
        comp.description = name + " component";
        
        breakdown.components[name] = comp;
        breakdown.totalScore += comp.weightedValue;
    }
    
    // Calculate unplaced squares for analysis
    breakdown.unplacedSquaresPlayer = calculateUnplacedSquares(state, true);
    breakdown.unplacedSquaresOpponent = calculateUnplacedSquares(state, false);
    
    return breakdown;
}

double EvaluationMetric::evaluateWinCondition(const Cathedral_state& state) {
    // CORE RULE: Player with fewest unplaced squares wins
    int playerUnplaced = calculateUnplacedSquares(state, true);
    int opponentUnplaced = calculateUnplacedSquares(state, false);
    
    // Normalize: positive is better for current player
    if (playerUnplaced + opponentUnplaced == 0) return 0.0;
    
    double ratio = static_cast<double>(opponentUnplaced - playerUnplaced) / 
                   static_cast<double>(playerUnplaced + opponentUnplaced);
    
    return ratio; // Range: [-1, 1]
}

double EvaluationMetric::evaluateTerritoryControl(const Cathedral_state& state) {
    // Territory advantage (wall-to-wall connections)
    int playerTerritory = countPlayerTerritory(state);
    int enemyTerritory = countEnemyTerritory(state);
    int totalTerritory = playerTerritory + enemyTerritory;
    
    if (totalTerritory == 0) return 0.0;
    
    return static_cast<double>(playerTerritory - enemyTerritory) / 
           static_cast<double>(totalTerritory + 1); // +1 to avoid division issues
}

double EvaluationMetric::evaluatePieceMobility(const Cathedral_state& state) {
    // How many pieces can still be placed and where
    auto playerMobility = countUnplayablePiecesForPlayer(state);
    
    // Create opponent state to check their mobility
    Cathedral_state tempState = state;
    tempState.change_turn();
    auto opponentMobility = countUnplayablePiecesForPlayer(tempState);
    
    // Calculate mobility advantage
    int playerPlayable = state.get_state_info().player1Shapes.size() - playerMobility.count;
    int opponentPlayable = state.get_state_info().player2Shapes.size() - opponentMobility.count;
    
    if (playerPlayable + opponentPlayable == 0) return 0.0;
    
    return static_cast<double>(playerPlayable - opponentPlayable) / 
           static_cast<double>(playerPlayable + opponentPlayable);
}

double EvaluationMetric::evaluatePositionalAdvantage(const Cathedral_state& state) {
    // Board control, connectivity, defensive positioning
    return assessPieceConnectivity(state) + evaluateDefensivePositioning(state);
}

double EvaluationMetric::evaluateThreatAssessment(const Cathedral_state& state) {
    // Capture threats and defensive needs
    return assessCaptureThreats(state);
}

double EvaluationMetric::evaluateCathedralStrategy(const Cathedral_state& state) {
    // Cathedral positioning and timing strategy
    bool cathedralOnBoard = checkCathedralOnBoard(state);
    if (!cathedralOnBoard) {
        // Cathedral not placed yet - this affects strategy significantly
        return 0.5; // Neutral until placed
    }
    
    // Evaluate cathedral positioning quality
    const auto& board = state.get_state_info().board;
    
    // Find cathedral position
    for (size_t i = 0; i < board.size(); ++i) {
        for (size_t j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == cathedral) {
                // Cathedral positioning heuristics
                double centerBonus = 0.0;
                int centerX = board.size() / 2;
                int centerY = board[0].size() / 2;
                int distFromCenter = abs(static_cast<int>(i) - centerX) + abs(static_cast<int>(j) - centerY);
                centerBonus = 1.0 - (static_cast<double>(distFromCenter) / (centerX + centerY));
                
                return centerBonus * 0.3; // Cathedral positioning is worth up to 30% of this component
            }
        }
    }
    
    return 0.0;
}

GamePhase EvaluationMetric::determineGamePhase(const Cathedral_state& state) {
    // Calculate how much of the game has progressed
    const auto& p1Shapes = state.get_state_info().player1Shapes;
    const auto& p2Shapes = state.get_state_info().player2Shapes;
    
    int totalOriginalPieces = 28; // Each player starts with 14 pieces
    int remainingPieces = p1Shapes.size() + p2Shapes.size();
    double progressRatio = 1.0 - (static_cast<double>(remainingPieces) / totalOriginalPieces);
    
    if (progressRatio < 0.2) return GamePhase::EARLY;
    if (progressRatio < 0.7) return GamePhase::MIDDLE;
    return GamePhase::LATE;
}

double EvaluationMetric::getPhaseWeight(const std::string& component, GamePhase phase) {
    auto it = PHASE_WEIGHTS.find(component);
    if (it != PHASE_WEIGHTS.end()) {
        auto phaseIt = it->second.find(phase);
        if (phaseIt != it->second.end()) {
            return phaseIt->second;
        }
    }
    return 1.0; // Default weight
}

int EvaluationMetric::calculateUnplacedSquares(const Cathedral_state& state, bool forCurrentPlayer) {
    // Calculate total squares in unplaced pieces
    const auto& shapes = forCurrentPlayer ? 
        (state.player1_turn() ? state.get_state_info().player1Shapes : state.get_state_info().player2Shapes) :
        (state.player1_turn() ? state.get_state_info().player2Shapes : state.get_state_info().player1Shapes);
    
    int totalSquares = 0;
    for (const auto& shape : shapes) {
        for (const auto& row : shape) {
            for (int cell : row) {
                if (cell != 0) totalSquares++;
            }
        }
    }
    
    return totalSquares;
}

double EvaluationMetric::assessPieceConnectivity(const Cathedral_state& state) {
    // Assess how well connected the player's pieces are
    const auto& board = state.get_state_info().board;
    int playerMin = state.player1_turn() ? player1Min : player2Min;
    int playerMax = state.player1_turn() ? player1Max : player2Max;
    
    int connections = 0;
    int totalPieces = 0;
    
    for (size_t i = 0; i < board.size(); ++i) {
        for (size_t j = 0; j < board[i].size(); ++j) {
            if (board[i][j] >= playerMin && board[i][j] <= playerMax) {
                totalPieces++;
                
                // Check adjacent cells for connections
                std::vector<std::pair<int, int>> directions = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                for (const auto& [dx, dy] : directions) {
                    int ni = i + dx, nj = j + dy;
                    if (ni >= 0 && ni < board.size() && nj >= 0 && nj < board[0].size()) {
                        if (board[ni][nj] >= playerMin && board[ni][nj] <= playerMax) {
                            connections++;
                        }
                    }
                }
            }
        }
    }
    
    return totalPieces > 0 ? static_cast<double>(connections) / (totalPieces * 4) : 0.0;
}

double EvaluationMetric::evaluateDefensivePositioning(const Cathedral_state& state) {
    // Evaluate how well positioned pieces are defensively
    // This is a simplified version - can be expanded
    return 0.0; // Placeholder
}

double EvaluationMetric::assessCaptureThreats(const Cathedral_state& state) {
    // Assess potential captures and threats
    // This is complex and requires analyzing potential enclosures
    return 0.0; // Placeholder
}

// Legacy function implementations (maintained for compatibility)
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
    return count > 0;
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
