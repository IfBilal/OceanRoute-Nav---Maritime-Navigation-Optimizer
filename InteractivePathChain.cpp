#include "InteractivePathChain.h"
#include <cmath>
#include <sstream>
#include <iomanip>

void InteractivePathChain::buildFromResult(PathResult& result, string algoName,
                                          int srcIdx, int dstIdx, DateTime start,
                                          bool optTime, string compFilter, double maxTime,
                                          Graph& graph) {
    // Clear existing chain
    clear();
    
    // Store algorithm info
    algorithmName = algoName;
    currentResult = result;
    sourceIdx = srcIdx;
    destIdx = dstIdx;
    startTime = start;
    optimizeTime = optTime;
    companyFilter = compFilter;
    maxVoyageTime = maxTime;
    avoidedPorts = Vector<string>();  // Reset avoided ports
    
    if (!result.pathFound || result.routes.empty()) {
        return;  // No path to display
    }
    
    // Build linked list from finalPathPorts (which is in reverse order)
    // finalPathPorts: [dest, ..., source], so we reverse it
    Vector<int> portIndices;
    for (int i = result.finalPathPorts.getSize() - 1; i >= 0; i--) {
        portIndices.push_back(result.finalPathPorts[i]);
    }
    
    // Calculate total width needed and center the chain
    int numNodes = portIndices.getSize();
    float totalWidth = (numNodes - 1) * NODE_SPACING;
    float chainStartX = (1600.0f - totalWidth) / 2.0f;  // Center horizontally
    
    // Create nodes
    float currentX = chainStartX;
    for (int i = 0; i < portIndices.getSize(); i++) {
        int portIdx = portIndices[i];
        string portName = graph.ports[portIdx]->name;
        
        PathChainNode* newNode = new PathChainNode(portName, portIdx);
        newNode->position = Vector2f(currentX, CHAIN_Y);
        newNode->bounds = FloatRect(currentX - NODE_RADIUS, CHAIN_Y - NODE_RADIUS,
                                    NODE_RADIUS * 2, NODE_RADIUS * 2);
        
        // Link nodes
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        
        currentX += NODE_SPACING;
    }
}

void InteractivePathChain::render(RenderWindow& window, Font& font) {
    if (!head) return;
    
    // Draw semi-transparent overlay for the chain area
    RectangleShape overlay(Vector2f(1600, 270));
    overlay.setPosition(0, 630);
    overlay.setFillColor(Color(5, 5, 15, 240));
    window.draw(overlay);
    
    // Draw centered heading
    Text heading;
    heading.setFont(font);
    heading.setString("MULTI-LEG ROUTE EDITOR");
    heading.setCharacterSize(28);
    heading.setFillColor(neonCyan);
    heading.setStyle(Text::Bold);
    FloatRect headingBounds = heading.getLocalBounds();
    heading.setOrigin(headingBounds.width / 2, 0);
    heading.setPosition(800, 640);
    window.draw(heading);
    
    // Draw centered path info
    stringstream ss;
    ss << fixed << setprecision(2);
    ss << "Cost: $" << currentResult.totalCost << "  |  Time: " << currentResult.totalTime << " hrs";
    if (!avoidedPorts.empty()) {
        ss << "  |  Avoiding: ";
        for (int i = 0; i < avoidedPorts.getSize(); i++) {
            ss << avoidedPorts[i];
            if (i < avoidedPorts.getSize() - 1) ss << ", ";
        }
    }
    
    Text infoLabel;
    infoLabel.setFont(font);
    infoLabel.setString(ss.str());
    infoLabel.setCharacterSize(16);
    infoLabel.setFillColor(Color(150, 200, 255));
    FloatRect infoBounds = infoLabel.getLocalBounds();
    infoLabel.setOrigin(infoBounds.width / 2, 0);
    infoLabel.setPosition(800, 675);
    window.draw(infoLabel);
    
    // Draw centered instruction
    Text instruction;
    instruction.setFont(font);
    if (isInAddMode) {
        instruction.setString("Type port name, press ENTER to add  |  ESC to cancel");
        instruction.setFillColor(neonPink);
    } else {
        instruction.setString("Click any node to modify path  |  Delete to avoid  |  Add to force through");
        instruction.setFillColor(Color(120, 150, 180));
    }
    instruction.setCharacterSize(14);
    FloatRect instrBounds = instruction.getLocalBounds();
    instruction.setOrigin(instrBounds.width / 2, 0);
    instruction.setPosition(800, 700);
    window.draw(instruction);
    
    // Draw arrows first (behind nodes)
    PathChainNode* curr = head;
    while (curr && curr->next) {
        Vector2f start = curr->position;
        Vector2f end = curr->next->position;
        
        // Glowing neon arrow line
        RectangleShape line;
        float length = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));
        float angle = atan2(end.y - start.y, end.x - start.x) * 180 / 3.14159f;
        
        // Outer glow
        RectangleShape glow;
        glow.setSize(Vector2f(length - NODE_RADIUS * 2, 6));
        glow.setPosition(start.x + NODE_RADIUS, start.y - 3);
        glow.setRotation(angle);
        glow.setFillColor(Color(0, 255, 255, 100));
        window.draw(glow);
        
        // Main line
        line.setSize(Vector2f(length - NODE_RADIUS * 2, 3));
        line.setPosition(start.x + NODE_RADIUS, start.y - 1.5f);
        line.setRotation(angle);
        line.setFillColor(neonCyan);
        window.draw(line);
        
        // Arrow head
        ConvexShape arrowHead;
        arrowHead.setPointCount(3);
        arrowHead.setPoint(0, Vector2f(0, -6));
        arrowHead.setPoint(1, Vector2f(12, 0));
        arrowHead.setPoint(2, Vector2f(0, 6));
        arrowHead.setPosition(end.x - NODE_RADIUS - 8, end.y);
        arrowHead.setFillColor(neonCyan);
        window.draw(arrowHead);
        
        curr = curr->next;
    }
    
    // Draw nodes
    curr = head;
    while (curr) {
        bool isSource = (curr == head);
        bool isDest = (curr == tail);
        
        // Outer glow for all nodes
        CircleShape outerGlow(NODE_RADIUS + 4);
        outerGlow.setPosition(curr->position.x - NODE_RADIUS - 4, 
                             curr->position.y - NODE_RADIUS - 4);
        outerGlow.setFillColor(curr->isSelected ? Color(255, 0, 255, 120) : Color(0, 255, 255, 80));
        window.draw(outerGlow);
        
        // Main node circle
        CircleShape node(NODE_RADIUS);
        node.setPosition(curr->position.x - NODE_RADIUS, curr->position.y - NODE_RADIUS);
        node.setFillColor(Color(15, 15, 35, 220));
        node.setOutlineThickness(2);
        
        // Different colors for source/dest/selected
        if (curr->isSelected) {
            node.setOutlineColor(neonPink);
        } else if (isSource) {
            node.setOutlineColor(Color(0, 255, 150));  // Green for source
        } else if (isDest) {
            node.setOutlineColor(Color(255, 100, 100));  // Red for dest
        } else {
            node.setOutlineColor(neonCyan);
        }
        window.draw(node);
        
        // Inner ring detail
        CircleShape innerRing(NODE_RADIUS * 0.7f);
        innerRing.setPosition(curr->position.x - NODE_RADIUS * 0.7f,
                             curr->position.y - NODE_RADIUS * 0.7f);
        innerRing.setFillColor(Color::Transparent);
        innerRing.setOutlineThickness(1);
        innerRing.setOutlineColor(Color(100, 100, 150, 150));
        window.draw(innerRing);
        
        // Port name
        Text portText;
        portText.setFont(font);
        portText.setString(curr->portName);
        portText.setCharacterSize(13);
        portText.setFillColor(Color(200, 220, 255));
        portText.setStyle(Text::Bold);
        
        FloatRect textBounds = portText.getLocalBounds();
        portText.setOrigin(textBounds.width / 2, textBounds.height / 2);
        portText.setPosition(curr->position.x, curr->position.y + NODE_RADIUS + 18);
        window.draw(portText);
        
        // Draw buttons if selected
        if (curr->showButtons) {
            float btnY = curr->position.y - NODE_RADIUS - BUTTON_SIZE - 8;
            
            // Only show delete button for middle nodes (not source/dest)
            if (!isSource && !isDest) {
                // Delete button (left)
                RectangleShape deleteBtn(Vector2f(BUTTON_SIZE, BUTTON_SIZE));
                deleteBtn.setPosition(curr->position.x - BUTTON_SIZE - 3, btnY);
                deleteBtn.setFillColor(Color(40, 10, 10, 230));
                deleteBtn.setOutlineThickness(2);
                deleteBtn.setOutlineColor(Color(255, 50, 50));
                window.draw(deleteBtn);
                
                // Delete icon (X)
                Text deleteIcon;
                deleteIcon.setFont(font);
                deleteIcon.setString("X");
                deleteIcon.setCharacterSize(20);
                deleteIcon.setFillColor(Color(255, 100, 100));
                deleteIcon.setStyle(Text::Bold);
                FloatRect delBounds = deleteIcon.getLocalBounds();
                deleteIcon.setOrigin(delBounds.width / 2, delBounds.height / 2);
                deleteIcon.setPosition(curr->position.x - BUTTON_SIZE/2 - 3, btnY + BUTTON_SIZE/2);
                window.draw(deleteIcon);
            }
            
            // Add button - show for ALL nodes (centered if source/dest, right side if middle)
            float addBtnX = (!isSource && !isDest) ? (curr->position.x + 3) : (curr->position.x - BUTTON_SIZE/2);
            RectangleShape addBtn(Vector2f(BUTTON_SIZE, BUTTON_SIZE));
            addBtn.setPosition(addBtnX, btnY);
            addBtn.setFillColor(Color(10, 40, 10, 230));
            addBtn.setOutlineThickness(2);
            addBtn.setOutlineColor(Color(50, 255, 50));
            window.draw(addBtn);
            
            // Add icon (+)
            Text addIcon;
            addIcon.setFont(font);
            addIcon.setString("+");
            addIcon.setCharacterSize(24);
            addIcon.setFillColor(Color(100, 255, 100));
            addIcon.setStyle(Text::Bold);
            FloatRect addBounds = addIcon.getLocalBounds();
            addIcon.setOrigin(addBounds.width / 2, addBounds.height / 2);
            addIcon.setPosition(addBtnX + BUTTON_SIZE/2, btnY + BUTTON_SIZE/2);
            window.draw(addIcon);
        }
        
        curr = curr->next;
    }
    
    // Draw add mode input box
    if (isInAddMode && addAfterNode) {
        float inputX = addAfterNode->position.x - 100;
        float inputY = addAfterNode->position.y - 120;
        
        // Input box
        RectangleShape inputBox(Vector2f(200, 40));
        inputBox.setPosition(inputX, inputY);
        inputBox.setFillColor(Color(20, 20, 40, 250));
        inputBox.setOutlineThickness(2);
        inputBox.setOutlineColor(neonPink);
        window.draw(inputBox);
        
        // Label
        Text label;
        label.setFont(font);
        label.setString("Port Name:");
        label.setCharacterSize(14);
        label.setFillColor(neonPink);
        label.setPosition(inputX + 5, inputY - 20);
        window.draw(label);
        
        // Input text
        Text inputText;
        inputText.setFont(font);
        inputText.setString(addPortInput);
        inputText.setCharacterSize(16);
        inputText.setFillColor(Color::White);
        inputText.setPosition(inputX + 5, inputY + 10);
        window.draw(inputText);
        
        // Cursor
        RectangleShape cursor(Vector2f(2, 24));
        FloatRect inputBounds = inputText.getLocalBounds();
        cursor.setPosition(inputX + 10 + inputBounds.width, inputY + 8);
        cursor.setFillColor(neonCyan);
        window.draw(cursor);
    }
    
    // Draw error message if active (auto-hide after 4 seconds)
    if (showError) {
        if (errorClock.getElapsedTime().asSeconds() > 4.0f) {
            showError = false;
        } else {
            // Error panel at top of chain area
            RectangleShape errorBg(Vector2f(600, 50));
            errorBg.setPosition(500, 720);
            errorBg.setFillColor(Color(80, 10, 10, 240));
            errorBg.setOutlineThickness(2);
            errorBg.setOutlineColor(Color(255, 50, 50));
            window.draw(errorBg);
            
            // Error icon
            Text errorIcon;
            errorIcon.setFont(font);
            errorIcon.setString("!");
            errorIcon.setCharacterSize(28);
            errorIcon.setFillColor(Color(255, 100, 100));
            errorIcon.setStyle(Text::Bold);
            errorIcon.setPosition(515, 728);
            window.draw(errorIcon);
            
            // Error text
            Text errorText;
            errorText.setFont(font);
            errorText.setString(errorMessage);
            errorText.setCharacterSize(16);
            errorText.setFillColor(Color(255, 200, 200));
            errorText.setStyle(Text::Bold);
            errorText.setPosition(550, 735);
            window.draw(errorText);
        }
    }
}

PathChainNode* InteractivePathChain::getNodeAtPosition(Vector2f mousePos) {
    PathChainNode* curr = head;
    while (curr) {
        if (curr->bounds.contains(mousePos)) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

void InteractivePathChain::handleClick(Vector2f mousePos, Graph& graph) {
    // Debug output
    cout << "[DEBUG] Chain click at (" << mousePos.x << ", " << mousePos.y << ")" << endl;
    
    // Don't handle node clicks if in add mode (text input active)
    if (isInAddMode) {
        cout << "[DEBUG] In add mode, ignoring click" << endl;
        return;
    }
    
    // Check if clicking on buttons first
    if (selectedNode && selectedNode->showButtons) {
        float btnY = selectedNode->position.y - NODE_RADIUS - BUTTON_SIZE - 8;
        bool isSource = (selectedNode == head);
        bool isDest = (selectedNode == tail);
        
        // Delete button area (left) - only for middle nodes
        if (!isSource && !isDest) {
            FloatRect deleteBounds(selectedNode->position.x - BUTTON_SIZE - 3, btnY,
                                  BUTTON_SIZE, BUTTON_SIZE);
            if (deleteBounds.contains(mousePos)) {
                cout << "[DEBUG] Delete button clicked!" << endl;
                deleteSelectedNode(graph);
                return;
            }
        }
        
        // Add button area - position depends on node type
        float addBtnX = (!isSource && !isDest) ? (selectedNode->position.x + 3) : (selectedNode->position.x - BUTTON_SIZE/2);
        FloatRect addBounds(addBtnX, btnY, BUTTON_SIZE, BUTTON_SIZE);
        if (addBounds.contains(mousePos)) {
            cout << "[DEBUG] Add button clicked!" << endl;
            startAddMode(selectedNode);
            return;
        }
    }
    
    // Check if clicking on a node
    PathChainNode* clicked = getNodeAtPosition(mousePos);
    if (clicked) {
        cout << "[DEBUG] Node clicked: " << clicked->portName << endl;
        
        // Allow clicking ANY node to show add button (even source/dest)
        // But only middle nodes can be deleted
        clearSelection();
        clicked->isSelected = true;
        clicked->showButtons = true;
        selectedNode = clicked;
        cout << "[DEBUG] Node selected, buttons shown" << endl;
    } else {
        cout << "[DEBUG] Empty space clicked, clearing selection" << endl;
        clearSelection();
    }
}

void InteractivePathChain::handleTextInput(char c) {
    if (!isInAddMode) return;
    
    // Handle backspace
    if (c == 8 && !addPortInput.empty()) {
        addPortInput.pop_back();
    }
    // Handle regular characters (letters, spaces, hyphens)
    else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ' || c == '-') {
        addPortInput += c;
    }
}

void InteractivePathChain::handleEnter(Graph& graph) {
    if (!isInAddMode || !addAfterNode) return;
    if (addPortInput.empty()) return;
    
    // Find port by name
    int portIndex = graph.findPortIndex(addPortInput);
    
    if (portIndex == -1) {
        // Port not found - show error and clear
        cout << "[ERROR] Port '" << addPortInput << "' not found!" << endl;
        showErrorMessage("Port '" + addPortInput + "' not found!");
        addPortInput = "";
        return;
    }
    
    // Check if port is already in path (avoid duplicates)
    PathChainNode* curr = head;
    while (curr) {
        if (curr->portIndex == portIndex) {
            cout << "[ERROR] Port '" << addPortInput << "' is already in the path!" << endl;
            showErrorMessage("Port '" + addPortInput + "' already in path!");
            addPortInput = "";
            return;
        }
        curr = curr->next;
    }
    
    // Add the node AFTER the selected node
    string newPortName = graph.ports[portIndex]->name;
    
    // Determine the source and destination for the two segments
    int segmentSource, segmentDest;
    DateTime segmentStartTime;
    
    // ALWAYS add AFTER the selected node (not between source and destination)
    segmentSource = addAfterNode->portIndex;
    segmentDest = addAfterNode->next ? addAfterNode->next->portIndex : destIdx;
    
    // For timing, we'd need to track arrival times at each node
    // For now, use start time (this is a simplification that may need improvement)
    segmentStartTime = startTime;
    
    cout << "[INFO] Adding '" << newPortName << "' after " << addAfterNode->portName << endl;
    
    // Two-segment pathfinding: selectedNode → newPort → nextNode
    PathResult path1 = graph.findPathDijkstra(
        segmentSource, portIndex, segmentStartTime, optimizeTime,
        companyFilter, avoidedPorts, maxVoyageTime
    );
    
    if (!path1.pathFound) {
        cout << "[ERROR] Cannot reach '" << newPortName << "' from " << graph.ports[segmentSource]->name << "!" << endl;
        showErrorMessage("Cannot reach '" + newPortName + "' from " + graph.ports[segmentSource]->name + "!");
        addPortInput = "";
        return;
    }
    
    // Calculate arrival time at new port
    DateTime arrivalAtNewPort = segmentStartTime;
    arrivalAtNewPort = arrivalAtNewPort.addHours(path1.totalTime);
    
    // Find path: newPort → segmentDest
    PathResult path2 = graph.findPathDijkstra(
        portIndex, segmentDest, arrivalAtNewPort, optimizeTime,
        companyFilter, avoidedPorts, maxVoyageTime
    );
    
    if (!path2.pathFound) {
        cout << "[ERROR] Cannot reach " << graph.ports[segmentDest]->name << " from '" << newPortName << "'!" << endl;
        showErrorMessage("Cannot reach " + graph.ports[segmentDest]->name + " from '" + newPortName + "'!");
        addPortInput = "";
        return;
    }
    
    // Combine paths (path1 routes + path2 routes)
    PathResult combinedResult;
    combinedResult.pathFound = true;
    combinedResult.totalCost = path1.totalCost + path2.totalCost;
    combinedResult.totalTime = path1.totalTime + path2.totalTime;
    
    // Merge routes using LinkedList iteration
    Node<Route>* routeNode = path1.routes.head;
    while (routeNode) {
        combinedResult.routes.insertAtEnd(routeNode->data);
        routeNode = routeNode->next;
    }
    routeNode = path2.routes.head;
    while (routeNode) {
        combinedResult.routes.insertAtEnd(routeNode->data);
        routeNode = routeNode->next;
    }
    
    // Merge finalPathPorts with duplicate detection
    // finalPathPorts is in REVERSE order: [dest, ..., source]
    // path1: [newPort, ..., segmentSource] (e.g., [Melbourne, Oslo])
    // path2: [segmentDest, ..., newPort] (e.g., [Shanghai, Melbourne])
    // We want: [segmentDest, ..., newPort, ..., segmentSource] (e.g., [Shanghai, Melbourne, Oslo])
    
    // Add all of path2 first
    for (int i = 0; i < path2.finalPathPorts.getSize(); i++) {
        combinedResult.finalPathPorts.push_back(path2.finalPathPorts[i]);
    }
    
    // Add path1 starting from index 1 (skip first element which is newPort)
    // BUT also check for duplicates in case path1 contains intermediate ports that path2 also has
    for (int i = 1; i < path1.finalPathPorts.getSize(); i++) {
        int portToAdd = path1.finalPathPorts[i];
        
        // Check if this port already exists in combinedResult
        bool alreadyExists = false;
        for (int j = 0; j < combinedResult.finalPathPorts.getSize(); j++) {
            if (combinedResult.finalPathPorts[j] == portToAdd) {
                alreadyExists = true;
                cout << "[DEBUG] Skipping duplicate port: " << graph.ports[portToAdd]->name << endl;
                break;
            }
        }
        
        // Only add if it doesn't already exist
        if (!alreadyExists) {
            combinedResult.finalPathPorts.push_back(portToAdd);
        }
    }
    
    // Rebuild chain with new combined result
    buildFromResult(combinedResult, algorithmName, sourceIdx, destIdx, 
                   startTime, optimizeTime, companyFilter, maxVoyageTime, graph);
    
    // Reset add mode
    isInAddMode = false;
    addAfterNode = nullptr;
    addPortInput = "";
    
    cout << "[OK] Added '" << newPortName << "' to path!" << endl;
}

void InteractivePathChain::cancelAddMode() {
    isInAddMode = false;
    addAfterNode = nullptr;
    addPortInput = "";
}

void InteractivePathChain::deleteSelectedNode(Graph& graph) {
    if (!selectedNode) return;
    
    // Cannot delete source or destination
    if (selectedNode == head || selectedNode == tail) {
        // Show error - cannot delete source/dest
        string nodeType = (selectedNode == head) ? "SOURCE" : "DESTINATION";
        showErrorMessage("Cannot delete " + nodeType + " port '" + selectedNode->portName + "'!");
        cout << "[ERROR] Cannot delete " << nodeType << " port '" << selectedNode->portName << "'!" << endl;
        clearSelection();
        return;
    }
    
    // Add to avoided ports
    avoidedPorts.push_back(selectedNode->portName);
    
    // Recalculate path
    PathResult newResult = graph.findPathDijkstra(
        sourceIdx, destIdx, startTime, optimizeTime,
        companyFilter, avoidedPorts, maxVoyageTime
    );
    
    if (!newResult.pathFound) {
        // No path exists - remove last avoided port and show error
        avoidedPorts.pop_back();
        showErrorMessage("No path exists avoiding '" + selectedNode->portName + "'!");
        cout << "[ERROR] No path exists avoiding '" << selectedNode->portName << "'!" << endl;
        clearSelection();
        return;
    }
    
    // Rebuild chain with new result
    buildFromResult(newResult, algorithmName, sourceIdx, destIdx, 
                   startTime, optimizeTime, companyFilter, maxVoyageTime, graph);
}

void InteractivePathChain::startAddMode(PathChainNode* afterNode) {
    isInAddMode = true;
    addAfterNode = afterNode;
    addPortInput = "";  // Clear input
    clearSelection();
}

void InteractivePathChain::clearSelection() {
    PathChainNode* curr = head;
    while (curr) {
        curr->isSelected = false;
        curr->showButtons = false;
        curr = curr->next;
    }
    selectedNode = nullptr;
}

void InteractivePathChain::showErrorMessage(string msg) {
    errorMessage = msg;
    showError = true;
    errorClock.restart();
}

void InteractivePathChain::clear() {
    PathChainNode* curr = head;
    while (curr) {
        PathChainNode* next = curr->next;
        delete curr;
        curr = next;
    }
    head = tail = selectedNode = nullptr;
    isInAddMode = false;
    addAfterNode = nullptr;
}