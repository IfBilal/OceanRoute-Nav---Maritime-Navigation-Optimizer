#pragma once
#include "Vector.h"
#include "LinkedList.h"
#include "Route.h"

struct PathResult {
    LinkedList<Route> routes;
    double totalCost;
    double totalTime;
    int nodesExplored;
    bool pathFound;
    Vector<int> explorationOrder;
    Vector<int> finalPathPorts;
    
    PathResult() : totalCost(0.0), totalTime(0.0), nodesExplored(0), pathFound(false) {}
};

struct AllPathsResult {
    Vector<PathResult> allPaths;  // All valid paths found
    int totalPathsFound;
    int nodesExplored;
    
    AllPathsResult() : totalPathsFound(0), nodesExplored(0) {}
};
