#pragma once

struct PathNode {
    double cost;
    int portIndex;
    
    PathNode() : cost(0.0), portIndex(-1) {}
    PathNode(double c, int idx) : cost(c), portIndex(idx) {}
    
    bool operator<(const PathNode& other) const {
        return cost > other.cost;
    }
    
    bool operator>(const PathNode& other) const {
        return cost < other.cost;
    }
};
