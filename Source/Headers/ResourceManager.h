#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture& getTexture(const std::string& name);
    
    void setResourcePath(const std::string& path);
    
private:
    ResourceManager() = default;
    std::unordered_map<std::string, sf::Texture> textures;
    std::string resourcePath;
    
    std::string getFullPath(const std::string& filename);
};

#endif