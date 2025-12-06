#pragma once
#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "Vector.h"     
#include "LinkedList.h" 
#include <cstdlib>
#include <ctime>

using namespace sf;

struct Particle {
    Vector2f position;
    Vector2f velocity;
    float size;
    Color color;
    float life;
    int connectionCount;  // Track connections for molecular network
};

struct RouteAnimation {
    int sourceIdx;
    int destIdx;
    float progress;
    float speed;    // Speed of drawing
    Color color;
    
    // Routes from file data
    string company;
    double cost;
    DateTime time;
    DateTime arrivalTime;  // Arrival time at destination
};

// Ship travel state - tracks which segment ship is on
struct ShipState {
    int currentSegmentIndex;  // Which route segment (0, 1, 2...)
    float segmentProgress;    // 0.0 to 1.0 within current segment
    bool active;
};

class Graphics {
private:
    RenderWindow& window; 
    Texture mapTexture;
    Sprite mapSprite;
    Font font;
    
    // Active animations list
    Vector<RouteAnimation> activeAnimations;
    
    // Ship movement with progressive path reveal
    Vector<RouteAnimation> shipAnimations;
    ShipState shipState;  // Current ship position
    
    // Particles for menu and effects
    Vector<Particle> particles;
    Clock particleClock;
    
    // Path visualization
    LinkedList<Route> currentPath;
    Color pathColor;
    bool pathVisible;
    
    // Rendered path segments (only show where ship has traveled)
    Vector<int> completedSegments;  // Track which segments to draw

    // Internal Helper Functions
    Vector2f getRelativeCoordinates(string cityName);
    float getDistance(Vector2f p1, Vector2f p2);
    Color getCompanyColor(const string& company);
    
    // Particle management
    void initParticles(int count);
    void updateParticles(float dt);
    void drawParticles();
    
    // Layered text rendering
    void drawTextLayered(const string& str, Vector2f pos, int size, Color mainColor, bool centered = false);

public:
    // Public for hover detection
    Vector<Vector2f> portScreenPositions;
    float distToSegment(Vector2f p, Vector2f v, Vector2f w);
    
    Graphics(RenderWindow& win, Graph& graph);

    // Call this every frame to update animation progress
    void update(float deltaTime);

    // Main Draw Loop - enhanced with source/dest highlighting
    void drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes, int sourceIdx = -1, int destIdx = -1);
    
    // Draw main menu screen
    void drawMainMenu(bool startButtonHovered);
    
    // Draw algorithm selection menu with particles
    void drawAlgorithmMenu(Vector<string>& algorithms, int hoveredIndex);
    
    // Draw algorithm options menu
    void drawAlgorithmOptionsMenu(bool useAvoidPorts, bool useMaxVoyageTime, 
                                   Vector<string>& avoidPorts, double maxVoyageTime);

    // HUD rendering
    void drawHUD(int sourceIdx, int destIdx, PathResult& result, Graph& graph, string state, string algorithm = "Dijkstra");
    
    // Traversal HUD - shows layover and route info during ship animation
    void drawTraversalHUD(Graph& graph, PathResult& pathResult);

    // Trigger an animation (e.g., when clicking a port)
    void startRouteAnimation(int startIdx, int endIdx, Route& data);
    
    // Set computed path for rendering
    void setComputedPath(LinkedList<Route>& path, Color color = Color(255, 215, 0));
    
    // Clear path visualization
    void clearPath();
    
    // Clears old animations
    void clearAnimations();
    
    // Ship movement along final path - PROGRESSIVE REVEAL
    void startShipAnimation(const LinkedList<Route>& path, Graph& graph);
    void setExplorationData(const PathResult& result);

    // Graph analysis view with filters
    void drawGraphAnalysis(Graph& graph, bool showFilterPanel, 
                          Vector<string>& continentFilters, Vector<bool>& continentActive,
                          Vector<string>& companyFilters, Vector<bool>& companyActive,
                          int hoveredPortIdx, int hoveredRouteSourceIdx, int hoveredRouteDestIdx,
                          Route& hoveredRoute);
    void drawFilterPanel(Vector<string>& continentFilters, Vector<bool>& continentActive,
                        Vector<string>& companyFilters, Vector<bool>& companyActive);
    void drawFilterButton(bool panelOpen);
    void drawPortTooltip(Port* port, Vector2f pos, Graph& graph);
    void drawRouteTooltip(Route& route, Vector2f mousePos, string sourceName, string destName);

    int handleMouseClick(int mouseX, int mouseY);
    void drawPortQueue(Vector2f pos, int count);
    void drawPath(LinkedList<Route>& path, Graph& graph);
    void drawDockingQueue(Graph& graph, int portIdx, Vector2f portPos);
    void drawShipsAtPorts(Graph& graph);
};