#pragma once
#include <SFML/Graphics.hpp>
#include "Vector.h"
#include "Graph.h"
#include "PathResult.h"

using namespace sf;

struct PathChainNode {
    string portName;
    int portIndex;
    
    // Visual properties
    Vector2f position;
    FloatRect bounds;
    bool isSelected;
    bool showButtons;
    
    // Linked list pointers
    PathChainNode* next;
    PathChainNode* prev;
    
    PathChainNode(string name, int idx) 
        : portName(name), portIndex(idx), isSelected(false), 
          showButtons(false), next(nullptr), prev(nullptr) {}
};

class InteractivePathChain {
private:
    PathChainNode* head;
    PathChainNode* tail;
    PathChainNode* selectedNode;
    
    // Algorithm info
    string algorithmName;
    PathResult currentResult;
    
    // Original query parameters (for recalculation)
    int sourceIdx;
    int destIdx;
    DateTime startTime;
    bool optimizeTime;
    string companyFilter;
    double maxVoyageTime;
    
    // Modification tracking
    Vector<string> avoidedPorts;
    
    // Visual constants
    const float CHAIN_Y = 765.0f;  // Center of bottom 30% (630 + 135)
    const float NODE_SPACING = 160.0f;
    const float NODE_RADIUS = 35.0f;
    const float BUTTON_SIZE = 35.0f;
    
    // Colors - Cyberpunk/Futuristic theme
    Color neonCyan = Color(0, 255, 255);
    Color neonPink = Color(255, 0, 255);
    Color neonPurple = Color(138, 43, 226);
    Color darkBg = Color(10, 10, 25, 200);
    Color glowColor = Color(0, 255, 255, 180);
    Color selectedGlow = Color(255, 0, 255, 200);
    Color buttonBg = Color(20, 20, 40, 230);
    Color buttonHover = Color(40, 40, 80, 250);
    
    // State
    bool isInAddMode;
    PathChainNode* addAfterNode;  // Node after which to add
    string addPortInput;  // Text input for port name
    
    // Error message system
    string errorMessage;
    Clock errorClock;
    bool showError;
    
public:
    InteractivePathChain() 
        : head(nullptr), tail(nullptr), selectedNode(nullptr),
          isInAddMode(false), addAfterNode(nullptr), addPortInput(""),
          sourceIdx(-1), destIdx(-1), optimizeTime(false), 
          maxVoyageTime(-1), showError(false), errorMessage("") {}
    
    ~InteractivePathChain() {
        clear();
    }
    
    // Build chain from PathResult
    void buildFromResult(PathResult& result, string algoName, 
                        int srcIdx, int dstIdx, DateTime start,
                        bool optTime, string compFilter, double maxTime,
                        Graph& graph);
    
    // Rendering
    void render(RenderWindow& window, Font& font);
    
    // Interaction
    void handleClick(Vector2f mousePos, Graph& graph);
    void handleTextInput(char c);
    void handleEnter(Graph& graph);
    void cancelAddMode();
    
    // Modification
    void deleteSelectedNode(Graph& graph);
    void startAddMode(PathChainNode* afterNode);
    
    // Utility
    PathChainNode* getNodeAtPosition(Vector2f mousePos);
    void clearSelection();
    void clear();
    bool isAddModeActive() const { return isInAddMode; }
    void showErrorMessage(string msg);
    PathResult getCurrentResult() const { return currentResult; }
    
    // Get path info for display
    string getAlgorithmName() const { return algorithmName; }
    double getTotalCost() const { return currentResult.totalCost; }
    double getTotalTime() const { return currentResult.totalTime; }
    int getNodeCount() const {
        int count = 0;
        PathChainNode* curr = head;
        while (curr) {
            count++;
            curr = curr->next;
        }
        return count;
    }
};
