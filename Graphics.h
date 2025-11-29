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

// Dijkstra exploration visualization
struct ExplorationStep {
    int portIdx;
    int fromIdx;
    float timestamp;  // When this exploration happened
    bool isInPath;    // True if this is part of the final path
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
    
    // Ship movement animations along final path
    ::vector<RouteAnimation> shipAnimations;
    
    // Dijkstra exploration visualization
    ::vector<ExplorationStep> explorationHistory;
    float explorationTime;
    bool isExploring;
    ::vector<int> finalPathPorts;  // Ports in the final solution path
    
    // Path visualization
    LinkedList<Route> currentPath;
    sf::Color pathColor;
    bool pathVisible;

    // Internal Helper Functions
    sf::Vector2f getRelativeCoordinates(std::string cityName);
    float getDistance(sf::Vector2f p1, sf::Vector2f p2);
    float distToSegment(sf::Vector2f p, sf::Vector2f v, sf::Vector2f w);
    sf::Color getCompanyColor(const string& company);

public:
    Graphics(sf::RenderWindow& win, Graph& graph);

    // Call this every frame to update animation progress
    void update(float deltaTime);

    // Main Draw Loop - enhanced with source/dest highlighting
    void drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes, int sourceIdx = -1, int destIdx = -1);

    // HUD rendering
    void drawHUD(int sourceIdx, int destIdx, PathResult& result, Graph& graph, string state, string algorithm = "Dijkstra");

    // Trigger an animation (e.g., when clicking a port)
    void startRouteAnimation(int startIdx, int endIdx, Route& data);
    
    // Set computed path for rendering
    void setComputedPath(LinkedList<Route>& path, sf::Color color = sf::Color(255, 215, 0));
    
    // Clear path visualization
    void clearPath();
    
    // Clears old animations
    void clearAnimations();
    
    // Dijkstra visualization
    void startExploration();
    void addExplorationStep(int portIdx, int fromIdx);
    void markFinalPath(int portIdx);
    void finishExploration();
    bool isExplorationActive() const { return isExploring; }
    void setExplorationData(const PathResult& result);  // New: set exploration from PathResult
    
    // Ship movement along final path
    void startShipAnimation(const LinkedList<Route>& path, Graph& graph);

    int handleMouseClick(int mouseX, int mouseY);
    void drawPortQueue(sf::Vector2f pos, int count);
    void drawPath(LinkedList<Route>& path, Graph& graph);
    void drawDockingQueue(Graph& graph, int portIdx, sf::Vector2f portPos);
    void drawShipsAtPorts(Graph& graph);
};