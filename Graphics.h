#pragma once
#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "Vector.h"     
#include "LinkedList.h" 

// Animation State for a single route being drawn
struct RouteAnimation {
    int sourceIdx;
    int destIdx;
    float progress; // 0.0 to 1.0
    float speed;    // Speed of drawing
    sf::Color color;
    
    // Routes from file data
    string company;
    double cost;
    DateTime time;
};

class Graphics {
private:
    sf::RenderWindow& window; 
    sf::Texture mapTexture;
    sf::Sprite mapSprite;
    sf::Font font;

    ::vector<sf::Vector2f> portScreenPositions; 
    
    // Active animations list
    ::vector<RouteAnimation> activeAnimations;

    // Internal Helper Functions
    sf::Vector2f getRelativeCoordinates(std::string cityName);
    float getDistance(sf::Vector2f p1, sf::Vector2f p2);
    float distToSegment(sf::Vector2f p, sf::Vector2f v, sf::Vector2f w);

public:
    Graphics(sf::RenderWindow& win, Graph& graph);

    // Call this every frame to update animation progress
    void update(float deltaTime);

    // Main Draw Loop
    void drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes);

    // Trigger an animation (e.g., when clicking a port)
    void startRouteAnimation(int startIdx, int endIdx, Route& data);
    
    // Clears old animations
    void clearAnimations();

    int handleMouseClick(int mouseX, int mouseY);
    void drawPortQueue(sf::Vector2f pos, int count);
    void drawPath(LinkedList<Route>& path);
};