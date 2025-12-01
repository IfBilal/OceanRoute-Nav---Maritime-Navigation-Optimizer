#pragma once
#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "Vector.h"     
#include "LinkedList.h" 

using namespace sf;

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
    RenderWindow& window; 
    Texture mapTexture;
    Sprite mapSprite;
    Font font;

    Vector<Vector2f> portScreenPositions; 
    
    // Active animations list
    Vector<RouteAnimation> activeAnimations;
    
    // Ship movement animations along final path
    Vector<RouteAnimation> shipAnimations;
    
    // Dijkstra exploration visualization
    Vector<ExplorationStep> explorationHistory;
    float explorationTime;
    bool isExploring;
    Vector<int> finalPathPorts;  // Ports in the final solution path
    
    // Path visualization
    LinkedList<Route> currentPath;
    Color pathColor;
    bool pathVisible;

    // Internal Helper Functions
    Vector2f getRelativeCoordinates(string cityName);
    float getDistance(Vector2f p1, Vector2f p2);
    float distToSegment(Vector2f p, Vector2f v, Vector2f w);
    Color getCompanyColor(const string& company);

public:
    Graphics(RenderWindow& win, Graph& graph);

    // Call this every frame to update animation progress
    void update(float deltaTime);

    // Main Draw Loop - enhanced with source/dest highlighting
    void drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes, int sourceIdx = -1, int destIdx = -1);

    // HUD rendering
    void drawHUD(int sourceIdx, int destIdx, PathResult& result, Graph& graph, string state, string algorithm = "Dijkstra");

    // Trigger an animation (e.g., when clicking a port)
    void startRouteAnimation(int startIdx, int endIdx, Route& data);
    
    // Set computed path for rendering
    void setComputedPath(LinkedList<Route>& path, Color color = Color(255, 215, 0));
    
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
    void drawPortQueue(Vector2f pos, int count);
    void drawPath(LinkedList<Route>& path, Graph& graph);
    void drawDockingQueue(Graph& graph, int portIdx, Vector2f portPos);
    void drawShipsAtPorts(Graph& graph);
};