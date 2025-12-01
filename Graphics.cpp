#include "Graphics.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace sf;

const Color CYBER_CYAN(0, 255, 255);
const Color CYBER_BLUE(0, 150, 255);
const Color CYBER_DARK(10, 15, 30, 240);
const Color GLOBAL_NET_COLOR(255, 180, 50, 40); 

const float ANIMATION_SPEED = 1.2f;

// --- Helper Functions ---
Vector2f Graphics::getRelativeCoordinates(string city) {
    // --- Americas ---
    if (city == "Vancouver")   return Vector2f(0.145f, 0.280f);
    if (city == "LosAngeles")  return Vector2f(0.155f, 0.380f);
    if (city == "Montreal")    return Vector2f(0.275f, 0.290f);
    if (city == "NewYork")     return Vector2f(0.285f, 0.340f);
    
    // --- Europe ---
    if (city == "Oslo")        return Vector2f(0.495f, 0.210f);
    if (city == "Stockholm")   return Vector2f(0.515f, 0.205f);
    if (city == "Helsinki")    return Vector2f(0.535f, 0.195f);
    if (city == "Copenhagen")  return Vector2f(0.495f, 0.240f);
    if (city == "Hamburg")     return Vector2f(0.490f, 0.260f);
    if (city == "Rotterdam")   return Vector2f(0.482f, 0.275f);
    if (city == "Antwerp")     return Vector2f(0.480f, 0.285f);
    if (city == "London")      return Vector2f(0.465f, 0.280f);
    if (city == "Dublin")      return Vector2f(0.445f, 0.270f);
    if (city == "Lisbon")      return Vector2f(0.435f, 0.360f);
    if (city == "Marseille")   return Vector2f(0.485f, 0.330f);
    if (city == "Genoa")       return Vector2f(0.495f, 0.320f);
    if (city == "Athens")      return Vector2f(0.530f, 0.360f);
    if (city == "Istanbul")    return Vector2f(0.555f, 0.350f);

    // --- Middle East / Africa ---
    if (city == "Alexandria")  return Vector2f(0.545f, 0.400f);
    if (city == "PortSaid")    return Vector2f(0.555f, 0.405f);
    if (city == "Jeddah")      return Vector2f(0.580f, 0.480f);
    if (city == "Dubai")       return Vector2f(0.640f, 0.460f);
    if (city == "AbuDhabi")    return Vector2f(0.635f, 0.465f);
    if (city == "Doha")        return Vector2f(0.630f, 0.455f);
    if (city == "CapeTown")    return Vector2f(0.535f, 0.840f);
    if (city == "Durban")      return Vector2f(0.580f, 0.800f);
    if (city == "PortLouis")   return Vector2f(0.660f, 0.750f);

    // --- Asia ---
    if (city == "Karachi")     return Vector2f(0.665f, 0.460f);
    if (city == "Mumbai")      return Vector2f(0.685f, 0.495f);
    if (city == "Colombo")     return Vector2f(0.710f, 0.560f);
    if (city == "Chittagong")  return Vector2f(0.755f, 0.480f);
    if (city == "Singapore")   return Vector2f(0.790f, 0.590f);
    if (city == "Jakarta")     return Vector2f(0.810f, 0.640f);
    if (city == "Manila")      return Vector2f(0.865f, 0.530f);
    if (city == "HongKong")    return Vector2f(0.840f, 0.480f);
    if (city == "Shanghai")    return Vector2f(0.860f, 0.430f);
    if (city == "Busan")       return Vector2f(0.880f, 0.380f);
    if (city == "Osaka")       return Vector2f(0.895f, 0.390f);
    if (city == "Tokyo")       return Vector2f(0.900f, 0.370f);

    // --- Oceania ---
    if (city == "Melbourne")   return Vector2f(0.875f, 0.810f);
    if (city == "Sydney")      return Vector2f(0.905f, 0.790f);

    return Vector2f(0.5f, 0.5f);
}

float Graphics::getDistance(Vector2f p1, Vector2f p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

float Graphics::distToSegment(Vector2f p, Vector2f v, Vector2f w) {
    float l2 = pow(getDistance(v, w), 2);
    if (l2 == 0) return getDistance(p, v);
    float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    t = max(0.0f, min(1.0f, t));
    Vector2f projection = v + t * (w - v);
    return getDistance(p, projection);
}

Color Graphics::getCompanyColor(const string& company) {
    // Distinct colors for each shipping company
    if (company == "MaerskLine") return Color(0, 150, 255);      // Blue
    if (company == "MSC") return Color(255, 50, 50);             // Red
    if (company == "CMA_CGM") return Color(255, 165, 0);         // Orange
    if (company == "COSCO") return Color(200, 0, 200);           // Purple
    if (company == "HapagLloyd") return Color(255, 200, 0);      // Yellow
    if (company == "Evergreen") return Color(0, 200, 100);       // Green
    if (company == "PIL") return Color(255, 100, 180);           // Pink
    if (company == "ZIM") return Color(100, 255, 255);           // Cyan
    if (company == "YangMing") return Color(255, 255, 100);      // Light Yellow
    if (company == "ONE") return Color(180, 100, 255);           // Lavender
    return Color(150, 150, 150);                                 // Gray default
}

Graphics::Graphics(RenderWindow& win, Graph& graph) : window(win) {
    if (!mapTexture.loadFromFile("assets/map.png")) {
        mapTexture.create(1600, 900); 
    }
    mapSprite.setTexture(mapTexture);
    float scaleX = (float)window.getSize().x / mapTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mapTexture.getSize().y;
    mapSprite.setScale(scaleX, scaleY);

    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {}
        }
    }

    for (int i = 0; i < graph.ports.getSize(); i++) {
        Vector2f relPos = getRelativeCoordinates(graph.ports[i]->name);
        portScreenPositions.push_back(Vector2f(relPos.x * window.getSize().x, relPos.y * window.getSize().y));
    }
    
    pathVisible = false;
    pathColor = Color(255, 215, 0);  // Gold
    isExploring = false;
    explorationTime = 0.0f;
}

// --- Animation Logic ---
void Graphics::startRouteAnimation(int startIdx, int endIdx, Route& data) {
    RouteAnimation anim;
    anim.sourceIdx = startIdx;
    anim.destIdx = endIdx;
    anim.progress = 0.0f;
    anim.speed = ANIMATION_SPEED;
    anim.color = CYBER_CYAN; 
    
    anim.company = data.company;
    anim.cost = data.voyageCost;
    anim.time = data.departureTime;

    activeAnimations.push_back(anim);
}

void Graphics::clearAnimations() {
    activeAnimations.clear();
    shipAnimations.clear();
    explorationHistory.clear();
    finalPathPorts.clear();
    isExploring = false;
    explorationTime = 0.0f;
}

void Graphics::update(float deltaTime) {
    // Update active route drawing animations
    for (int i = 0; i < activeAnimations.getSize(); i++) {
        if (activeAnimations[i].progress < 1.0f) {
            activeAnimations[i].progress += activeAnimations[i].speed * deltaTime;
            if (activeAnimations[i].progress > 1.0f) activeAnimations[i].progress = 1.0f;
        }
    }
    
    // Update ship movement animations along final path
    for (int i = 0; i < shipAnimations.getSize(); i++) {
        if (shipAnimations[i].progress < 1.0f) {
            shipAnimations[i].progress += shipAnimations[i].speed * deltaTime;
            if (shipAnimations[i].progress > 1.0f) shipAnimations[i].progress = 1.0f;
        }
    }
    
    // Update exploration visualization
    if (isExploring) {
        explorationTime += deltaTime;
    }
}

// --- Drawing ---
void Graphics::drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes, int sourceIdx, int destIdx) {
    window.draw(mapSprite);
    Vector2f mousePos(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
    string activeTooltip = "";

    // 1. Draw Global Routes (If 'T' is pressed) - Color-coded by Company
    if (showAllRoutes) {
        for (int i = 0; i < graph.ports.getSize(); i++) {
            auto current = graph.ports[i]->routes.head;
            while (current != nullptr) {
                int destIdx = current->data.destinationIndex;
                
                // Get company-specific color for route
                Color routeColor = getCompanyColor(current->data.company);
                routeColor.a = 60; // Semi-transparent
                
                Vertex line[] = {
                    Vertex(portScreenPositions[i], routeColor), 
                    Vertex(portScreenPositions[destIdx], routeColor)
                };
                window.draw(line, 2, Lines);
                
                // Draw tiny ship icon at midpoint showing company
                Vector2f midpoint = (portScreenPositions[i] + portScreenPositions[destIdx]) * 0.5f;
                RectangleShape tinyShip(Vector2f(4, 2));
                tinyShip.setPosition(midpoint);
                tinyShip.setFillColor(routeColor);
                window.draw(tinyShip);
                
                current = current->next;
            }
        }
    }

    // 2. Draw Active Animations (Glowing Cyan)
    for (int i = 0; i < activeAnimations.getSize(); i++) {
        RouteAnimation& anim = activeAnimations[i];
        Vector2f start = portScreenPositions[anim.sourceIdx];
        Vector2f end = portScreenPositions[anim.destIdx];
        Vector2f currentTip = start + (end - start) * anim.progress;

        Vertex line[] = {
            Vertex(start, anim.color),
            Vertex(currentTip, Color(0, 150, 255, 150)) 
        };
        
        window.draw(line, 2, Lines);
        line[0].position.x += 1; line[1].position.x += 1; 
        window.draw(line, 2, Lines);
        line[0].position.y += 1; line[1].position.y += 1; 
        window.draw(line, 2, Lines);
        
        CircleShape head(3.0f);
        head.setOrigin(3.0f, 3.0f);
        head.setPosition(currentTip);
        head.setFillColor(Color::White);
        window.draw(head);

        if (activeTooltip.empty() && distToSegment(mousePos, start, end) < 8.0f) {
             stringstream ss;
             ss << "ROUTE: " << graph.ports[anim.sourceIdx]->name << " -> " << graph.ports[anim.destIdx]->name << "\n"
                << "Company: " << anim.company << "\n"
                << "Cost: $" << anim.cost;
             activeTooltip = ss.str();
        }
    }
    
    // 2.5. Draw Computed Path (if visible)
    if (pathVisible && !currentPath.empty()) {
        drawPath(currentPath, graph);
    }
    
    // 2.6. Draw Dijkstra Exploration Animation
    if (isExploring && !explorationHistory.empty()) {
        float currentTime = explorationTime;
        
        for (int i = 0; i < explorationHistory.getSize(); i++) {
            ExplorationStep& step = explorationHistory[i];
            
            // ONLY DRAW FINAL PATH - skip exploration steps
            if (!step.isInPath) continue;
            
            // Only draw steps that have been "revealed" by time
            if (step.timestamp > currentTime) break;
            
            // Calculate fade based on time since exploration
            float timeSinceExplore = currentTime - step.timestamp;
            float alpha = 255.0f * max(0.0f, 1.0f - timeSinceExplore / 3.0f); // Fade over 3 seconds
            
            // Draw connection from previous port with animation
            if (step.fromIdx != -1) {
                Vector2f start = portScreenPositions[step.fromIdx];
                Vector2f end = portScreenPositions[step.portIdx];
                
                // Calculate progress of the dot moving from source to destination
                // Progress is 0 to 1 over 1 second window
                float moveProgress = min(1.0f, timeSinceExplore);
                
                // Interpolate position
                Vector2f animatedPos = Vector2f(
                    start.x + (end.x - start.x) * moveProgress,
                    start.y + (end.y - start.y) * moveProgress
                );
                
                Color lineColor = Color(255, 215, 0, static_cast<Uint8>(alpha)); // Gold for final path
                
                // Draw the line to show the path taken
                Vertex line[] = {
                    Vertex(start, lineColor),
                    Vertex(end, lineColor)
                };
                window.draw(line, 2, Lines);
                
                // Draw moving dot along the connection
                if (moveProgress < 1.0f) {
                    CircleShape movingDot(10.0f);
                    movingDot.setOrigin(10.0f, 10.0f);
                    movingDot.setPosition(animatedPos);
                    movingDot.setFillColor(Color(255, 215, 0, 255)); // Gold moving dot for final path
                    movingDot.setOutlineThickness(3.0f);
                    movingDot.setOutlineColor(Color(255, 255, 255, 255));
                    window.draw(movingDot);
                }
            }
            
            // Draw final path port with pulsing effect (ONLY for final path)
            Vector2f pos = portScreenPositions[step.portIdx];
            float pulsePhase = (currentTime - step.timestamp) * 4.0f;
            float pulseSize = 12.0f + 6.0f * sin(pulsePhase);
            
            CircleShape exploredPort(pulseSize);
            exploredPort.setOrigin(pulseSize, pulseSize);
            exploredPort.setPosition(pos);
            exploredPort.setFillColor(Color(255, 215, 0, static_cast<Uint8>(alpha)));
            exploredPort.setOutlineColor(Color(255, 255, 0, static_cast<Uint8>(alpha)));
            exploredPort.setOutlineThickness(2);
            
            window.draw(exploredPort);
        }
    }
    
    // 2.7. Draw Ship Animations Along Final Path
    for (int i = 0; i < shipAnimations.getSize(); i++) {
        RouteAnimation& ship = shipAnimations[i];
        
        // Skip if animation hasn't started yet (negative progress = delay)
        if (ship.progress < 0.0f) continue;
        
        // Skip if animation is complete
        if (ship.progress >= 1.0f) continue;
        
        Vector2f start = portScreenPositions[ship.sourceIdx];
        Vector2f end = portScreenPositions[ship.destIdx];
        Vector2f shipPos = start + (end - start) * ship.progress;
        
        // Draw ship trail (faint line behind ship)
        Vertex trail[] = {
            Vertex(start, Color(ship.color.r, ship.color.g, ship.color.b, 80)),
            Vertex(shipPos, ship.color)
        };
        window.draw(trail, 2, Lines);
        
        // Draw ship as glowing circle with company color
        float shipSize = 8.0f;
        CircleShape shipGlow(shipSize + 3.0f);
        shipGlow.setOrigin(shipSize + 3.0f, shipSize + 3.0f);
        shipGlow.setPosition(shipPos);
        shipGlow.setFillColor(Color(ship.color.r, ship.color.g, ship.color.b, 100));
        window.draw(shipGlow);
        
        CircleShape shipCore(shipSize);
        shipCore.setOrigin(shipSize, shipSize);
        shipCore.setPosition(shipPos);
        shipCore.setFillColor(ship.color);
        shipCore.setOutlineColor(Color::White);
        shipCore.setOutlineThickness(2);
        window.draw(shipCore);
    }

    // 3. Draw Ports with Source/Dest highlighting
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        Vector2f pos = portScreenPositions[i];
        
        static Clock pulseClock;
        float time = pulseClock.getElapsedTime().asSeconds();
        float pulseScale = 1.0f + 0.2f * sin(time * 3.0f + i); 
        
        bool isSource = (i == sourceIdx);
        bool isDest = (i == destIdx);
        bool isSelected = (i == selectedPortIndex);
        
        // Larger halos for source/dest
        float haloSize = (isSource || isDest) ? 10.0f : 7.0f;
        float coreSize = (isSource || isDest) ? 5.0f : 3.0f;
        
        CircleShape halo(haloSize * pulseScale);
        halo.setOrigin(halo.getRadius(), halo.getRadius());
        halo.setPosition(pos);
        
        CircleShape core(coreSize);
        core.setOrigin(coreSize, coreSize);
        core.setPosition(pos);

        if (isSource) {
            // GREEN for source
            halo.setFillColor(Color(0, 255, 0, 150));
            halo.setOutlineColor(Color(0, 255, 0));
            halo.setOutlineThickness(2);
            core.setFillColor(Color(0, 255, 0));
            
            // Label
            Text label("SRC", font, 11);
            label.setPosition(pos.x - 14, pos.y - 28);
            label.setFillColor(Color(0, 255, 0));
            label.setStyle(Text::Bold);
            window.draw(label);
        }
        else if (isDest) {
            // RED for destination
            halo.setFillColor(Color(255, 0, 0, 150));
            halo.setOutlineColor(Color(255, 0, 0));
            halo.setOutlineThickness(2);
            core.setFillColor(Color(255, 0, 0));
            
            // Label
            Text label("DEST", font, 11);
            label.setPosition(pos.x - 17, pos.y - 28);
            label.setFillColor(Color(255, 0, 0));
            label.setStyle(Text::Bold);
            window.draw(label);
        }
        else if (isSelected) {
            halo.setFillColor(Color(0, 255, 255, 120)); 
            halo.setOutlineColor(CYBER_CYAN);
            halo.setOutlineThickness(1);
            core.setFillColor(Color::White);
        } else {
            halo.setFillColor(Color(0, 100, 200, 50)); 
            core.setFillColor(CYBER_CYAN);
        }

        window.draw(halo);
        window.draw(core);

        if (getDistance(mousePos, portScreenPositions[i]) < 15.0f) {
            stringstream ss;
            ss << "PORT: " << graph.ports[i]->name << "\n"
               << "Charges: $" << graph.ports[i]->portCharges;
            activeTooltip = ss.str();
            
            CircleShape hoverRing(12.0f);
            hoverRing.setOrigin(12.0f, 12.0f);
            hoverRing.setPosition(pos);
            hoverRing.setFillColor(Color::Transparent);
            hoverRing.setOutlineColor(Color::White);
            hoverRing.setOutlineThickness(1);
            window.draw(hoverRing);
        }
    }
    
    // 3.5. Draw Ships at Ports (Docking Queues)
    drawShipsAtPorts(graph);

    // 4. Draw Tooltip
    if (!activeTooltip.empty()) {
        Text text(activeTooltip, font, 14);
        FloatRect bounds = text.getGlobalBounds();
        RectangleShape bg(Vector2f(bounds.width + 20, bounds.height + 16));
        bg.setFillColor(CYBER_DARK);
        bg.setOutlineColor(CYBER_CYAN);
        bg.setOutlineThickness(1);
        bg.setPosition(mousePos.x + 15, mousePos.y + 15);
        text.setPosition(mousePos.x + 25, mousePos.y + 20);
        text.setFillColor(CYBER_CYAN);
        window.draw(bg);
        window.draw(text);
    }
}

int Graphics::handleMouseClick(int mouseX, int mouseY) {
    Vector2f mousePos(mouseX, mouseY);
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        if (getDistance(mousePos, portScreenPositions[i]) < 20.0f) return i;
    }
    return -1;
}

// Enhanced docking queue visualization for a specific port
void Graphics::drawDockingQueue(Graph& graph, int portIdx, Vector2f portPos) {
    if (portIdx < 0 || portIdx >= graph.ports.getSize()) return;
    
    Port* port = graph.ports[portIdx];
    if (port->shipsQueue.isEmpty()) return;
    
    // Count ships in queue
    int shipCount = 0;
    Queue<Ship> tempQueue = port->shipsQueue;
    Vector<string> companies; // Store company names for coloring
    
    while (!tempQueue.isEmpty()) {
        Ship ship = tempQueue.front();
        companies.push_back(ship.companyName);
        tempQueue.dequeue();
        shipCount++;
    }
    
    // Draw up to 5 tiny ship rectangles stacked vertically
    int maxDisplay = (shipCount > 5) ? 5 : shipCount;
    float rectWidth = 8.0f;
    float rectHeight = 4.0f;
    float spacing = 1.0f;
    float startX = portPos.x + 12;
    float startY = portPos.y - 20;
    
    for (int i = 0; i < maxDisplay; i++) {
        RectangleShape shipRect(Vector2f(rectWidth, rectHeight));
        shipRect.setPosition(startX, startY - (i * (rectHeight + spacing)));
        
        // Color by company
        Color shipColor = getCompanyColor(companies[i]);
        shipRect.setFillColor(shipColor);
        shipRect.setOutlineColor(Color::White);
        shipRect.setOutlineThickness(0.5f);
        
        window.draw(shipRect);
    }
    
    // If more than 5 ships, show count
    if (shipCount > 5) {
        Text countLabel("+" + to_string(shipCount - 5), font, 10);
        countLabel.setPosition(startX + rectWidth + 2, startY - (maxDisplay * (rectHeight + spacing)) + 2);
        countLabel.setFillColor(Color(255, 255, 0));
        countLabel.setStyle(Text::Bold);
        countLabel.setOutlineColor(Color::Black);
        countLabel.setOutlineThickness(1);
        window.draw(countLabel);
    }
}

void Graphics::drawPortQueue(Vector2f pos, int count) {
    if (count <= 0) return;
    
    // Simple queue indicator (used for quick overview)
    CircleShape queueIndicator(6);
    queueIndicator.setPosition(pos.x + 8, pos.y - 14);
    queueIndicator.setFillColor(Color(255, 200, 0, 180));
    queueIndicator.setOutlineColor(Color(255, 255, 0));
    queueIndicator.setOutlineThickness(1);
    window.draw(queueIndicator);
    
    // Count badge
    Text countText(to_string(count), font, 9);
    countText.setPosition(pos.x + 10, pos.y - 13);
    countText.setFillColor(Color::Black);
    countText.setStyle(Text::Bold);
    window.draw(countText);
}

// Draw all ships currently at ports across the map
void Graphics::drawShipsAtPorts(Graph& graph) {
    // Only draw queues for ports in the final path
    for (int i = 0; i < graph.ports.getSize(); i++) {
        Port* port = graph.ports[i];
        if (port->shipsQueue.isEmpty()) continue;
        
        // Check if this port is in the final path
        bool isInPath = false;
        for (int j = 0; j < finalPathPorts.getSize(); j++) {
            if (finalPathPorts[j] == i) {
                isInPath = true;
                break;
            }
        }
        
        // Only draw queue if port is in the final path
        if (!isInPath) continue;
        
        Vector2f portPos = portScreenPositions[i];
        
        // Draw detailed docking queue for this port
        drawDockingQueue(graph, i, portPos);
    }
}

// Set computed path for rendering
void Graphics::setComputedPath(LinkedList<Route>& path, Color color) {
    currentPath = path;
    pathColor = color;
    pathVisible = true;
}

// Clear path visualization
void Graphics::clearPath() {
    currentPath.clear();
    pathVisible = false;
}

// Draw computed path with glow effect and layover indicators
void Graphics::drawPath(LinkedList<Route>& path, Graph& graph) {
    if (path.empty()) return;
    
    Node<Route>* current = path.head;
    
    while (current != nullptr) {
        int srcIdx = graph.findPortIndex(current->data.sourcePortName);
        int destIdx = current->data.destinationIndex;
        
        if (srcIdx == -1 || destIdx == -1) {
            current = current->next;
            continue;
        }
        
        Vector2f start = portScreenPositions[srcIdx];
        Vector2f end = portScreenPositions[destIdx];
        
        // Get company color for this route
        Color routeColor = getCompanyColor(current->data.company);
        routeColor.a = 255;
        
        // Multi-layer glow effect for path
        // Layer 1: Outer glow (thick, faint)
        for (int offset = -6; offset <= 6; offset++) {
            Vertex outerGlow[] = {
                Vertex(Vector2f(start.x, start.y + offset), Color(routeColor.r, routeColor.g, routeColor.b, 30)),
                Vertex(Vector2f(end.x, end.y + offset), Color(routeColor.r, routeColor.g, routeColor.b, 30))
            };
            window.draw(outerGlow, 2, Lines);
        }
        
        // Layer 2: Mid glow
        for (int offset = -3; offset <= 3; offset++) {
            Vertex midGlow[] = {
                Vertex(Vector2f(start.x, start.y + offset), Color(routeColor.r, routeColor.g, routeColor.b, 100)),
                Vertex(Vector2f(end.x, end.y + offset), Color(routeColor.r, routeColor.g, routeColor.b, 100))
            };
            window.draw(midGlow, 2, Lines);
        }
        
        // Layer 3: Core bright line
        Vertex core[] = {
            Vertex(start, routeColor),
            Vertex(end, routeColor)
        };
        window.draw(core, 2, Lines);
        for (int i = 0; i < 2; i++) {
            core[0].position.x += 1;
            core[1].position.x += 1;
            window.draw(core, 2, Lines);
        }
        
        // Draw ship icon on the route showing company
        Vector2f shipPos = (start + end) * 0.5f;
        RectangleShape shipIcon(Vector2f(10, 5));
        shipIcon.setPosition(shipPos.x - 5, shipPos.y - 2.5f);
        shipIcon.setFillColor(routeColor);
        shipIcon.setOutlineColor(Color::White);
        shipIcon.setOutlineThickness(1);
        window.draw(shipIcon);
        
        // Company label
        Text companyLabel(current->data.company, font, 9);
        companyLabel.setPosition(shipPos.x - 15, shipPos.y - 20);
        companyLabel.setFillColor(routeColor);
        companyLabel.setOutlineColor(Color::Black);
        companyLabel.setOutlineThickness(1);
        companyLabel.setStyle(Text::Bold);
        window.draw(companyLabel);
        
        // Cost label
        Text costLabel("$" + to_string((int)current->data.voyageCost), font, 10);
        costLabel.setPosition(shipPos.x + 15, shipPos.y - 8);
        costLabel.setFillColor(Color(255, 215, 0));
        costLabel.setOutlineColor(Color::Black);
        costLabel.setOutlineThickness(1);
        window.draw(costLabel);
        
        // Calculate and show layover time at destination
        Ship arrivalShip(
            current->data.arrivalTime,
            current->data.company,
            current->data.destinationPortName,
            current->data.sourcePortName
        );
        double waitHours = graph.freeTime(graph.ports[destIdx], arrivalShip);
        
        // Layover feasibility indicator at destination port
        CircleShape layoverIndicator(8);
        layoverIndicator.setPosition(end.x + 10, end.y + 10);
        layoverIndicator.setOrigin(8, 8);
        
        if (waitHours > 12.0) {
            // RED: Long layover with charges
            layoverIndicator.setFillColor(Color(255, 50, 50, 200));
            layoverIndicator.setOutlineColor(Color(255, 0, 0));
            layoverIndicator.setOutlineThickness(2);
            
            // Warning icon (!)
            Text warning("!", font, 12);
            warning.setPosition(end.x + 6, end.y + 5);
            warning.setFillColor(Color::White);
            warning.setStyle(Text::Bold);
            window.draw(warning);
        } else if (waitHours > 6.0) {
            // YELLOW: Moderate layover
            layoverIndicator.setFillColor(Color(255, 200, 0, 200));
            layoverIndicator.setOutlineColor(Color(255, 255, 0));
            layoverIndicator.setOutlineThickness(1);
        } else {
            // GREEN: Short layover, optimal
            layoverIndicator.setFillColor(Color(0, 255, 100, 200));
            layoverIndicator.setOutlineColor(Color(0, 255, 0));
            layoverIndicator.setOutlineThickness(1);
        }
        window.draw(layoverIndicator);
        
        // Layover time label
        Text layoverLabel(to_string((int)waitHours) + "h", font, 8);
        layoverLabel.setPosition(end.x + 20, end.y + 12);
        layoverLabel.setFillColor(Color::White);
        layoverLabel.setOutlineColor(Color::Black);
        layoverLabel.setOutlineThickness(1);
        window.draw(layoverLabel);
        
        current = current->next;
    }
}

// Draw HUD panel
void Graphics::drawHUD(int sourceIdx, int destIdx, PathResult& result, Graph& graph, string state, string algorithm) {
    // HUD Background Panel - smaller and at bottom left
    RectangleShape hudBg(Vector2f(280, 160));
    hudBg.setPosition(10, 900 - 170);  // Bottom left (10px from left, 170px from bottom)
    hudBg.setFillColor(Color(10, 15, 30, 200));
    hudBg.setOutlineColor(CYBER_CYAN);
    hudBg.setOutlineThickness(1);
    window.draw(hudBg);
    
    float startX = 18;
    float startY = 900 - 165;
    
    // Title - smaller
    Text title("OCEANROUTE NAV", font, 12);
    title.setPosition(startX, startY);
    title.setFillColor(CYBER_CYAN);
    title.setStyle(Text::Bold);
    window.draw(title);
    
    // State indicator - compact
    Text stateText(state, font, 10);
    stateText.setPosition(startX + 140, startY + 2);
    
    if (state == "IDLE" || state == "READY TO COMPUTE") {
        stateText.setFillColor(Color(150, 150, 150));
    } else if (state.find("SELECTING") != string::npos) {
        stateText.setFillColor(Color(255, 255, 0));
    } else if (state == "COMPUTING...") {
        stateText.setFillColor(Color(255, 140, 0));
    } else if (state == "PATH FOUND") {
        stateText.setFillColor(Color(0, 255, 0));
    }
    window.draw(stateText);
    
    // Algorithm name
    Text algoText("ALGO: " + algorithm, font, 10);
    algoText.setPosition(startX, startY + 15);
    algoText.setFillColor(Color(255, 215, 0));
    window.draw(algoText);
    
    // Separator line
    RectangleShape line(Vector2f(260, 1));
    line.setPosition(startX, startY + 30);
    line.setFillColor(CYBER_CYAN);
    window.draw(line);
    
    // Source & Destination - compact
    if (sourceIdx != -1) {
        Text srcText("SRC: " + graph.ports[sourceIdx]->name, font, 10);
        srcText.setPosition(startX, startY + 37);
        srcText.setFillColor(Color(0, 255, 0));
        window.draw(srcText);
    }
    
    if (destIdx != -1) {
        Text destText("DST: " + graph.ports[destIdx]->name, font, 10);
        destText.setPosition(startX, startY + 52);
        destText.setFillColor(Color(255, 0, 0));
        window.draw(destText);
    }
    
    // Results (if path computed)
    if (result.pathFound) {
        RectangleShape separator(Vector2f(260, 1));
        separator.setPosition(startX, startY + 67);
        separator.setFillColor(Color(80, 80, 80));
        window.draw(separator);
        
        Text costText("Cost: $" + to_string((int)result.totalCost), font, 11);
        costText.setPosition(startX, startY + 74);
        costText.setFillColor(Color::White);
        costText.setStyle(Text::Bold);
        window.draw(costText);
        
        Text timeText("Time: " + to_string((int)result.totalTime) + "h", font, 10);
        timeText.setPosition(startX, startY + 92);
        timeText.setFillColor(Color(150, 200, 255));
        window.draw(timeText);
        
        // Hops count
        int hops = 0;
        Node<Route>* counter = result.routes.head;
        while (counter) {
            hops++;
            counter = counter->next;
        }
        
        Text nodesText("Explored: " + to_string(result.nodesExplored) + " | Hops: " + to_string(hops), font, 10);
        nodesText.setPosition(startX, startY + 110);
        nodesText.setFillColor(Color(100, 200, 255));
        window.draw(nodesText);
    }
}

// === DIJKSTRA VISUALIZATION ===
void Graphics::startExploration() {
    explorationHistory.clear();
    explorationTime = 0.0f;
    isExploring = true;
}

void Graphics::addExplorationStep(int portIdx, int fromIdx) {
    ExplorationStep step;
    step.portIdx = portIdx;
    step.fromIdx = fromIdx;
    step.timestamp = explorationTime;
    step.isInPath = false;
    explorationHistory.push_back(step);
    explorationTime += 0.05f; // 50ms per exploration step
}

void Graphics::markFinalPath(int portIdx) {
    // Mark this port as part of the final path
    for (int i = 0; i < explorationHistory.getSize(); i++) {
        if (explorationHistory[i].portIdx == portIdx) {
            explorationHistory[i].isInPath = true;
        }
    }
}

void Graphics::finishExploration() {
    isExploring = false;
}

void Graphics::setExplorationData(const PathResult& result) {
    explorationHistory.clear();
    explorationTime = 0.0f;
    isExploring = true;
    
    // Store final path ports for queue visualization
    finalPathPorts.clear();
    for (int i = 0; i < result.finalPathPorts.getSize(); i++) {
        finalPathPorts.push_back(result.finalPathPorts[i]);
    }
    
    // ONLY add steps for the final path ports, not all explored ports
    for (int i = 0; i < result.finalPathPorts.getSize(); i++) {
        ExplorationStep step;
        step.portIdx = result.finalPathPorts[i];
        step.fromIdx = (i > 0) ? result.finalPathPorts[i-1] : -1; // Connect to previous port in FINAL PATH
        step.timestamp = i * 1.0f; // 1 second per step
        step.isInPath = true; // All steps are in the final path now
        
        explorationHistory.push_back(step);
    }
}

void Graphics::startShipAnimation(const LinkedList<Route>& path, Graph& graph) {
    shipAnimations.clear();
    
    // Create ship animations for each route segment in the path
    Node<Route>* current = path.head;
    float delay = 0.0f; // Stagger animations
    
    while (current != nullptr) {
        // Find source port index from name
        int sourceIdx = graph.findPortIndex(current->data.sourcePortName);
        if (sourceIdx == -1) {
            current = current->next;
            continue; // Skip if port not found
        }
        
        RouteAnimation ship;
        ship.sourceIdx = sourceIdx;
        ship.destIdx = current->data.destinationIndex;
        ship.progress = -delay; // Negative progress = delay before starting
        ship.speed = 0.3f; // Slower ship movement (was 1.2f for route drawing)
        ship.color = getCompanyColor(current->data.company);
        ship.company = current->data.company;
        ship.cost = current->data.voyageCost;
        ship.time = current->data.departureTime;
        
        shipAnimations.push_back(ship);
        
        delay += 0.5f; // 0.5 second delay between each hop
        current = current->next;
    }
}

