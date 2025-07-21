//This class is the overarching game truth. It is designed to keep CathedralState small and fast for mcts. 
#include "CathedralState.h"

class GameState {
    public:
        Cathedral_state cathedralState;

        GameState(Cathedral_state& state) : cathedralState(state) {};


};