#include "Headers/ResourceManager.h"
#include <iostream>
#include <filesystem>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    std::string fullPath = getFullPath(filename);
    
    if (!texture.loadFromFile(fullPath)) {
        std::cerr << "Failed to load texture: " << fullPath << std::endl;
        return false;
    }
    
    textures[name] = std::move(texture);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& name) {
    return textures[name];
}

void ResourceManager::setResourcePath(const std::string& path) {
    resourcePath = path;
}

std::string ResourceManager::getFullPath(const std::string& filename) {
    if (resourcePath.empty()) {
        // Auto-detect resource path relative to executable
        std::filesystem::path execPath = std::filesystem::current_path();
        if (std::filesystem::exists(execPath / "Source" / "Images")) {
            resourcePath = (execPath / "Source" / "Images").string();
        } else if (std::filesystem::exists(execPath / "Images")) {
            resourcePath = (execPath / "Images").string();
        }
    }
    
    return std::filesystem::path(resourcePath) / filename;
}