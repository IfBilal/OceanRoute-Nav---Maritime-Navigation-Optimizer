#pragma once
#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "Vector.h"     
#include "LinkedList.h" 

class Graphics {
private:
    sf::RenderWindow& window; 
    sf::Texture mapTexture;
    sf::Sprite mapSprite;
    sf::Font font;

    // Global namespace vector
    ::vector<sf::Vector2f> portScreenPositions; 

    // Internal Helper Functions
    sf::Vector2f getRelativeCoordinates(std::string cityName);
    float getDistance(sf::Vector2f p1, sf::Vector2f p2);
    float distToSegment(sf::Vector2f p, sf::Vector2f v, sf::Vector2f w);

public:
    // Constructor
    Graphics(sf::RenderWindow& win, Graph& graph);

    // Main Draw Loop
    void drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes);

    // Future Features
    void drawPath(LinkedList<Route>& path);
    
    // The function causing you trouble - declared here
    void drawPortQueue(sf::Vector2f pos, int count); 
    
    int handleMouseClick(int mouseX, int mouseY);
};