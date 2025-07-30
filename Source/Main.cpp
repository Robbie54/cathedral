#include <SFML/Graphics.hpp>


#include "Headers/ScreenRenderer.h"
#include "Headers/Global.h"
#include "Headers/PlayerTurn.h"
#include "Headers/CathedralState.h"
#include "Headers/MatrixUtility.h"
#include "Headers/ResourceManager.h"
#include "../MonteCarloTreeSearch-main/mcts/include/mcts.h"



#include <iostream>
#include <thread>
#include <future>

using namespace std;

#include "Headers/GameManager.h"

int main() {
    ResourceManager& rm = ResourceManager::getInstance();
    
    // Load all textures once at startup
    if (!rm.loadTexture("misc", "misc.png")) {
        std::cerr << "Failed to load required textures!" << std::endl;
        return -1;
    }
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * SCREEN_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT) * SCREEN_RESIZE), "Cathedral", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, GRID_SIZE * SCREEN_WIDTH, FONT_HEIGHT + GRID_SIZE * SCREEN_HEIGHT)));
    window.setPosition(sf::Vector2i(100, 100));

    GameManager manager(window);
    manager.run();

    return 0;
}