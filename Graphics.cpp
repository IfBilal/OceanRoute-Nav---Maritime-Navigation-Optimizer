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
    srand(time(NULL));
    
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
    pathColor = Color(255, 215, 0);
    
    // Initialize ship state
    shipState.active = false;
    shipState.currentSegmentIndex = 0;
    shipState.segmentProgress = 0.0f;
    
    // Initialize particles for menu
    initParticles(250);
}

void Graphics::initParticles(int count) {
    particles.clear();
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position.x = rand() % 1600;
        p.position.y = rand() % 900;
        // Slower, more organic movement
        p.velocity.x = -20.0f + (rand() % 100) / 100.0f * 40.0f;
        p.velocity.y = -20.0f + (rand() % 100) / 100.0f * 40.0f;
        p.size = 2.0f + (rand() % 30) / 10.0f;
        
        // More varied colors for molecular look
        int colorChoice = rand() % 5;
        if (colorChoice == 0) {
            p.color = Color(0, 255, 255, 180);  // Cyan
        } else if (colorChoice == 1) {
            p.color = Color(0, 200, 255, 150);  // Blue
        } else if (colorChoice == 2) {
            p.color = Color(100, 150, 255, 160);  // Light blue
        } else if (colorChoice == 3) {
            p.color = Color(150, 255, 200, 140);  // Mint
        } else {
            p.color = Color(200, 150, 255, 130);  // Purple
        }
        
        p.life = 1.0f;
        p.connectionCount = 0;
        particles.push_back(p);
    }
}

void Graphics::updateParticles(float dt) {
    for (int i = 0; i < particles.getSize(); i++) {
        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;
        
        // Wrap around screen
        if (particles[i].position.x < 0) particles[i].position.x = 1600;
        if (particles[i].position.x > 1600) particles[i].position.x = 0;
        if (particles[i].position.y < 0) particles[i].position.y = 900;
        if (particles[i].position.y > 900) particles[i].position.y = 0;
    }
}

void Graphics::drawParticles() {
    const float CONNECTION_DISTANCE = 120.0f;  // Max distance for connections
    const int MAX_CONNECTIONS = 5;  // Max connections per particle
    
    // First pass: reset connection counts
    for (int i = 0; i < particles.getSize(); i++) {
        particles[i].connectionCount = 0;
    }
    
    // Second pass: draw connections (molecular network)
    for (int i = 0; i < particles.getSize(); i++) {
        if (particles[i].connectionCount >= MAX_CONNECTIONS) continue;
        
        for (int j = i + 1; j < particles.getSize(); j++) {
            if (particles[j].connectionCount >= MAX_CONNECTIONS) continue;
            
            float dist = getDistance(particles[i].position, particles[j].position);
            
            if (dist < CONNECTION_DISTANCE) {
                // Calculate alpha based on distance (closer = brighter)
                float alpha = (1.0f - (dist / CONNECTION_DISTANCE)) * 80.0f;
                
                // Create gradient line
                Color color1 = particles[i].color;
                Color color2 = particles[j].color;
                color1.a = alpha;
                color2.a = alpha;
                
                Vertex line[] = {
                    Vertex(particles[i].position, color1),
                    Vertex(particles[j].position, color2)
                };
                window.draw(line, 2, Lines);
                
                particles[i].connectionCount++;
                particles[j].connectionCount++;
                
                if (particles[i].connectionCount >= MAX_CONNECTIONS) break;
            }
        }
    }
    
    // Third pass: draw particles (nodes)
    for (int i = 0; i < particles.getSize(); i++) {
        // Main particle
        CircleShape particle(particles[i].size);
        particle.setPosition(particles[i].position.x - particles[i].size, 
                           particles[i].position.y - particles[i].size);
        particle.setFillColor(particles[i].color);
        window.draw(particle);
        
        // Glow effect (stronger for more connected nodes)
        float glowMultiplier = 1.0f + (particles[i].connectionCount * 0.3f);
        CircleShape glow(particles[i].size * 2.5f * glowMultiplier);
        glow.setPosition(particles[i].position.x - particles[i].size * 2.5f * glowMultiplier, 
                        particles[i].position.y - particles[i].size * 2.5f * glowMultiplier);
        Color glowColor = particles[i].color;
        glowColor.a = 25 + (particles[i].connectionCount * 5);
        glow.setFillColor(glowColor);
        window.draw(glow);
        
        // Core highlight for high-connectivity nodes
        if (particles[i].connectionCount >= 3) {
            CircleShape core(particles[i].size * 0.5f);
            core.setPosition(particles[i].position.x - particles[i].size * 0.5f, 
                           particles[i].position.y - particles[i].size * 0.5f);
            core.setFillColor(Color(255, 255, 255, 200));
            window.draw(core);
        }
    }
}

void Graphics::drawTextLayered(const string& str, Vector2f pos, int size, Color mainColor, bool centered) {
    Text text(str, font, size);
    
    if (centered) {
        FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }
    
    // Layer 1: Outer glow (large, very faint)
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx == 0 && dy == 0) continue;
            text.setPosition(pos.x + dx, pos.y + dy);
            Color glow = mainColor;
            glow.a = 20;
            text.setFillColor(glow);
            window.draw(text);
        }
    }
    
    // Layer 2: Outline
    text.setPosition(pos.x - 1, pos.y - 1);
    text.setFillColor(Color::Black);
    window.draw(text);
    text.setPosition(pos.x + 1, pos.y + 1);
    window.draw(text);
    
    // Layer 3: Main text
    text.setPosition(pos);
    text.setFillColor(mainColor);
    window.draw(text);
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
    anim.arrivalTime = data.arrivalTime;

    activeAnimations.push_back(anim);
}

void Graphics::clearAnimations() {
    activeAnimations.clear();
    shipAnimations.clear();
    completedSegments.clear();
    shipState.active = false;
    shipState.currentSegmentIndex = 0;
    shipState.segmentProgress = 0.0f;
}

void Graphics::update(float deltaTime) {
    // Update particles for main menu
    updateParticles(deltaTime);
    
    // Update active route drawing animations
    for (int i = 0; i < activeAnimations.getSize(); i++) {
        if (activeAnimations[i].progress < 1.0f) {
            activeAnimations[i].progress += activeAnimations[i].speed * deltaTime;
            if (activeAnimations[i].progress > 1.0f) activeAnimations[i].progress = 1.0f;
        }
    }
    
    // Update ship movement with progressive path reveal
    if (shipState.active && !shipAnimations.empty()) {
        int segIdx = shipState.currentSegmentIndex;
        
        if (segIdx < shipAnimations.getSize()) {
            // Move ship along current segment
            shipState.segmentProgress += shipAnimations[segIdx].speed * deltaTime;
            
            // Segment complete - move to next
            if (shipState.segmentProgress >= 1.0f) {
                shipState.segmentProgress = 0.0f;
                completedSegments.push_back(segIdx);  // Mark segment as completed
                shipState.currentSegmentIndex++;
                
                // Check if journey complete
                if (shipState.currentSegmentIndex >= shipAnimations.getSize()) {
                    shipState.active = false;
                }
            }
        }
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
    
    // 2.6. Draw ONLY completed path segments (where ship has already traveled)
    for (int i = 0; i < completedSegments.getSize(); i++) {
        int segIdx = completedSegments[i];
        if (segIdx >= shipAnimations.getSize()) continue;
        
        RouteAnimation& seg = shipAnimations[segIdx];
        Vector2f start = portScreenPositions[seg.sourceIdx];
        Vector2f end = portScreenPositions[seg.destIdx];
        
        // Draw glowing completed path segment
        for (int offset = -4; offset <= 4; offset++) {
            Vertex line[] = {
                Vertex(Vector2f(start.x, start.y + offset), Color(seg.color.r, seg.color.g, seg.color.b, 50)),
                Vertex(Vector2f(end.x, end.y + offset), Color(seg.color.r, seg.color.g, seg.color.b, 50))
            };
            window.draw(line, 2, Lines);
        }
        
        // Core line
        Vertex core[] = {
            Vertex(start, seg.color),
            Vertex(end, seg.color)
        };
        window.draw(core, 2, Lines);
    }
    
    // 2.7. Draw CURRENT path segment being traveled (progressive reveal)
    if (shipState.active && shipState.currentSegmentIndex < shipAnimations.getSize()) {
        RouteAnimation& currentSeg = shipAnimations[shipState.currentSegmentIndex];
        Vector2f start = portScreenPositions[currentSeg.sourceIdx];
        Vector2f end = portScreenPositions[currentSeg.destIdx];
        
        // Draw partial segment from start to current ship position
        Vector2f shipPos = start + (end - start) * shipState.segmentProgress;
        
        // Glow effect on current segment
        for (int offset = -3; offset <= 3; offset++) {
            Vertex line[] = {
                Vertex(Vector2f(start.x, start.y + offset), Color(currentSeg.color.r, currentSeg.color.g, currentSeg.color.b, 80)),
                Vertex(Vector2f(shipPos.x, shipPos.y + offset), Color(currentSeg.color.r, currentSeg.color.g, currentSeg.color.b, 80))
            };
            window.draw(line, 2, Lines);
        }
        
        // Core line
        Vertex core[] = {
            Vertex(start, currentSeg.color),
            Vertex(shipPos, currentSeg.color)
        };
        window.draw(core, 2, Lines);
        
        // Draw ship at current position
        float shipSize = 10.0f;
        CircleShape shipGlow(shipSize + 4.0f);
        shipGlow.setOrigin(shipSize + 4.0f, shipSize + 4.0f);
        shipGlow.setPosition(shipPos);
        shipGlow.setFillColor(Color(currentSeg.color.r, currentSeg.color.g, currentSeg.color.b, 100));
        window.draw(shipGlow);
        
        CircleShape shipCore(shipSize);
        shipCore.setOrigin(shipSize, shipSize);
        shipCore.setPosition(shipPos);
        shipCore.setFillColor(currentSeg.color);
        shipCore.setOutlineColor(Color::White);
        shipCore.setOutlineThickness(3);
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
    
    // Draw up to 5 ship rectangles stacked vertically - BIGGER
    int maxDisplay = (shipCount > 5) ? 5 : shipCount;
    float rectWidth = 14.0f;
    float rectHeight = 6.0f;
    float spacing = 2.0f;
    float startX = portPos.x + 12;
    float startY = portPos.y - 25;
    
    for (int i = 0; i < maxDisplay; i++) {
        RectangleShape shipRect(Vector2f(rectWidth, rectHeight));
        shipRect.setPosition(startX, startY - (i * (rectHeight + spacing)));
        
        // Color by company
        Color shipColor = getCompanyColor(companies[i]);
        shipRect.setFillColor(shipColor);
        shipRect.setOutlineColor(Color::White);
        shipRect.setOutlineThickness(1.0f);
        
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

void Graphics::drawShipsAtPorts(Graph& graph) {
    if (!shipState.active || shipAnimations.empty()) return;
    
    int currentSegment = shipState.currentSegmentIndex;
    
    // Only draw docking queue for:
    // 1. Current destination port (where ship is heading)
    // 2. Source port of current segment if ship just started
    if (currentSegment >= 0 && currentSegment < shipAnimations.getSize()) {
        RouteAnimation& seg = shipAnimations[currentSegment];
        
        // Draw destination port's queue (where ship is going)
        int destIdx = seg.destIdx;
        if (destIdx >= 0 && destIdx < graph.ports.getSize()) {
            Port* destPort = graph.ports[destIdx];
            if (!destPort->shipsQueue.isEmpty()) {
                Vector2f destPos = portScreenPositions[destIdx];
                drawDockingQueue(graph, destIdx, destPos);
            }
        }
        
        // Only show source queue if ship just started this segment (first 10% of journey)
        if (shipState.segmentProgress < 0.1f) {
            int srcIdx = seg.sourceIdx;
            if (srcIdx >= 0 && srcIdx < graph.ports.getSize()) {
                Port* srcPort = graph.ports[srcIdx];
                if (!srcPort->shipsQueue.isEmpty()) {
                    Vector2f srcPos = portScreenPositions[srcIdx];
                    drawDockingQueue(graph, srcIdx, srcPos);
                }
            }
        }
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

void Graphics::drawMainMenu(bool startButtonHovered) {
    // Dark cyberpunk background
    RectangleShape darkBg(Vector2f(1600, 900));
    darkBg.setFillColor(Color(5, 8, 15));
    window.draw(darkBg);
    
    // Draw floating particle network
    drawParticles();
    
    // Animated scanlines
    static float scanlineOffset = 0.0f;
    scanlineOffset += 0.5f;
    if (scanlineOffset > 4.0f) scanlineOffset = 0.0f;
    
    for (int y = 0; y < 900; y += 4) {
        RectangleShape scanline(Vector2f(1600, 1));
        scanline.setPosition(0, y + scanlineOffset);
        scanline.setFillColor(Color(0, 255, 255, 5));
        window.draw(scanline);
    }
    
    // Grid background
    for (int x = 0; x < 1600; x += 50) {
        Vertex line[] = {
            Vertex(Vector2f(x, 0), Color(0, 100, 150, 15)),
            Vertex(Vector2f(x, 900), Color(0, 100, 150, 15))
        };
        window.draw(line, 2, Lines);
    }
    for (int y = 0; y < 900; y += 50) {
        Vertex line[] = {
            Vertex(Vector2f(0, y), Color(0, 100, 150, 15)),
            Vertex(Vector2f(1600, y), Color(0, 100, 150, 15))
        };
        window.draw(line, 2, Lines);
    }
    
    // Pulsing title
    static float titlePulse = 0.0f;
    titlePulse += 0.02f;
    float titleGlow = 200 + 55 * sin(titlePulse);
    
    Text title("OCEANROUTE NAV", font, 52);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, 0);
    title.setPosition(800, 140);
    title.setFillColor(Color(0, 255, 255, titleGlow));
    
    // Glow effect on title
    for (int offset = 1; offset <= 3; offset++) {
        Text titleGlowLayer("OCEANROUTE NAV", font, 52);
        titleGlowLayer.setOrigin(titleBounds.width / 2, 0);
        titleGlowLayer.setPosition(800, 140);
        titleGlowLayer.setFillColor(Color(0, 200, 255, 30 / offset));
        titleGlowLayer.setScale(1.0f + offset * 0.02f, 1.0f + offset * 0.02f);
        window.draw(titleGlowLayer);
    }
    window.draw(title);
    
    Text subtitle("Maritime Navigation Optimizer", font, 18);
    FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setOrigin(subtitleBounds.width / 2, 0);
    subtitle.setPosition(800, 210);
    subtitle.setFillColor(Color(100, 200, 255, 180));
    window.draw(subtitle);
    
    // Instruction with glow
    Text instruction("SELECT PATHFINDING ALGORITHM", font, 16);
    FloatRect instrBounds = instruction.getLocalBounds();
    instruction.setOrigin(instrBounds.width / 2, 0);
    instruction.setPosition(800, 270);
    instruction.setFillColor(Color(0, 255, 255, 150));
    window.draw(instruction);
    
    // Algorithm buttons with cyberpunk style
    Vector<string> algoNames;
    algoNames.push_back("BOOK ROUTES");
    algoNames.push_back("GRAPH ANALYSIS");
    algoNames.push_back("DFS (ALL PATHS)");
    
    float buttonY = 340;
    float buttonWidth = 420;
    float buttonHeight = 65;
    float spacing = 18;
    float buttonX = 800 - buttonWidth / 2;
    
    Vector2i mousePos = Mouse::getPosition(window);
    
    for (int i = 0; i < 3; i++) {
        float y = buttonY + i * (buttonHeight + spacing);
        
        FloatRect bounds(buttonX, y, buttonWidth, buttonHeight);
        bool isHovered = bounds.contains(mousePos.x, mousePos.y);
        
        // Outer glow
        if (isHovered) {
            RectangleShape glow(Vector2f(buttonWidth + 8, buttonHeight + 8));
            glow.setPosition(buttonX - 4, y - 4);
            glow.setFillColor(Color::Transparent);
            glow.setOutlineColor(Color(0, 255, 255, 80));
            glow.setOutlineThickness(2);
            window.draw(glow);
        }
        
        RectangleShape button(Vector2f(buttonWidth, buttonHeight));
        button.setPosition(buttonX, y);
        
        if (isHovered) {
            button.setFillColor(Color(0, 80, 120, 220));
            button.setOutlineColor(Color(0, 255, 255, 255));
            button.setOutlineThickness(2);
        } else {
            button.setFillColor(Color(10, 25, 40, 200));
            button.setOutlineColor(Color(0, 150, 200, 180));
            button.setOutlineThickness(1);
        }
        
        window.draw(button);
        
        // Corner accents
        float cornerSize = 12;
        for (int corner = 0; corner < 4; corner++) {
            float cx = (corner % 2 == 0) ? buttonX : buttonX + buttonWidth;
            float cy = (corner < 2) ? y : y + buttonHeight;
            
            RectangleShape accent1(Vector2f(cornerSize, 2));
            accent1.setPosition(cx + (corner % 2 == 0 ? 0 : -cornerSize), cy);
            accent1.setFillColor(Color(0, 255, 255, isHovered ? 255 : 150));
            window.draw(accent1);
            
            RectangleShape accent2(Vector2f(2, cornerSize));
            accent2.setPosition(cx, cy + (corner < 2 ? 0 : -cornerSize));
            accent2.setFillColor(Color(0, 255, 255, isHovered ? 255 : 150));
            window.draw(accent2);
        }
        
        // Algorithm text
        Text algoText(algoNames[i], font, 20);
        FloatRect algoBounds = algoText.getLocalBounds();
        algoText.setOrigin(algoBounds.width / 2, 0);
        algoText.setPosition(800, y + 22);
        algoText.setFillColor(isHovered ? Color(0, 255, 255) : Color(150, 200, 255));
        if (isHovered) algoText.setStyle(Text::Bold);
        window.draw(algoText);
    }
    
    // Animated footer with pulse
    float footerAlpha = 100 + 50 * sin(titlePulse * 2);
    Text controls("CONTROLS: [F5] RESET | [ESC] EXIT", font, 13);
    FloatRect controlBounds = controls.getLocalBounds();
    controls.setOrigin(controlBounds.width / 2, 0);
    controls.setPosition(800, 740);
    controls.setFillColor(Color(0, 200, 255, footerAlpha));
    window.draw(controls);
    
    // Tech corner decorations
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner % 2 == 0) ? 20 : 1580;
        float cy = (corner < 2) ? 20 : 880;
        
        RectangleShape deco1(Vector2f(40, 2));
        deco1.setPosition(cx + (corner % 2 == 0 ? 0 : -40), cy);
        deco1.setFillColor(Color(0, 255, 255, 100));
        window.draw(deco1);
        
        RectangleShape deco2(Vector2f(2, 40));
        deco2.setPosition(cx, cy + (corner < 2 ? 0 : -40));
        deco2.setFillColor(Color(0, 255, 255, 100));
        window.draw(deco2);
    }
}

void Graphics::drawAlgorithmMenu(Vector<string>& algorithms, int hoveredIndex) {
    // Dark futuristic background
    RectangleShape darkBg(Vector2f(1600, 900));
    darkBg.setFillColor(Color(5, 10, 25));
    window.draw(darkBg);
    
    // Draw molecular network particles
    drawParticles();
    
    // Hexagonal grid background (futuristic)
    static float hexPhase = 0.0f;
    hexPhase += 0.005f;
    
    for (int x = -100; x < 1700; x += 80) {
        for (int y = -100; y < 1000; y += 70) {
            float offsetX = (y / 70) % 2 == 0 ? 0 : 40;
            float px = x + offsetX;
            float py = y;
            
            ConvexShape hexagon(6);
            float radius = 25;
            for (int i = 0; i < 6; i++) {
                float angle = i * 60 * 3.14159f / 180.0f;
                hexagon.setPoint(i, Vector2f(
                    px + radius * cos(angle),
                    py + radius * sin(angle)
                ));
            }
            hexagon.setFillColor(Color::Transparent);
            float pulseAlpha = 10 + 8 * sin(hexPhase + px * 0.01f + py * 0.01f);
            hexagon.setOutlineColor(Color(0, 150, 200, pulseAlpha));
            hexagon.setOutlineThickness(1);
            window.draw(hexagon);
        }
    }
    
    // TITLE: "WELCOME TO OCEANROUTE NAV"
    static float titlePulse = 0.0f;
    titlePulse += 0.02f;
    
    float titleScale = 1.0f + 0.05f * sin(titlePulse);
    int titleSize = 48 * titleScale;
    
    Color titleColor(0, 255, 255);
    titleColor.a = 200 + 55 * sin(titlePulse);
    
    drawTextLayered("WELCOME TO", Vector2f(800, 150), 32, Color(255, 215, 0), true);
    drawTextLayered("OCEANROUTE NAV", Vector2f(800, 210), titleSize, titleColor, true);
    drawTextLayered("MARITIME PATHFINDING SYSTEM", Vector2f(800, 270), 18, Color(100, 200, 255), true);
    
    // Subtitle
    drawTextLayered("SELECT YOUR PATHFINDING ALGORITHM", Vector2f(800, 320), 20, Color(200, 200, 200), true);
    
    // Algorithm boxes
    float startY = 380;
    float boxWidth = 400;
    float boxHeight = 50;
    float spacing = 10;
    float startX = 800 - boxWidth / 2;
    
    for (int i = 0; i < algorithms.getSize(); i++) {
        float y = startY + i * (boxHeight + spacing);
        
        RectangleShape box(Vector2f(boxWidth, boxHeight));
        box.setPosition(startX, y);
        
        if (i == hoveredIndex) {
            box.setOutlineThickness(2);
            box.setOutlineColor(Color(150, 150, 150));
            box.setFillColor(Color(40, 40, 50, 220));
        } else {
            box.setOutlineThickness(1);
            box.setOutlineColor(Color(80, 80, 80));
            box.setFillColor(Color(20, 20, 30, 200));
        }
        
        window.draw(box);
        
        // Algorithm text
        Color textColor = (i == hoveredIndex) ? Color(200, 200, 200) : Color(150, 150, 150);
        drawTextLayered(algorithms[i], Vector2f(800, y + 15), 18, textColor, true);
    }
    
    // Footer hint
    drawTextLayered("Click to select", Vector2f(800, 750), 14, Color(120, 120, 120), true);
}

// Draw algorithm options menu
void Graphics::drawAlgorithmOptionsMenu(bool useAvoidPorts, bool useMaxVoyageTime, 
                                         Vector<string>& avoidPorts, double maxVoyageTime) {
    // Dark overlay
    RectangleShape overlay(Vector2f(1600, 900));
    overlay.setFillColor(Color(10, 10, 15, 220));
    window.draw(overlay);
    
    // Title
    Text title("ALGORITHM OPTIONS", font, 32);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, 0);
    title.setPosition(800, 120);
    title.setFillColor(Color(200, 200, 200));
    window.draw(title);
    
    // Instructions
    Text instruction("Configure pathfinding constraints", font, 14);
    FloatRect instrBounds = instruction.getLocalBounds();
    instruction.setOrigin(instrBounds.width / 2, 0);
    instruction.setPosition(800, 170);
    instruction.setFillColor(Color(120, 120, 120));
    window.draw(instruction);
    
    // Options list
    float startY = 240;
    float spacing = 25;
    int line = 0;
    
    // [1] Avoid ports
    Text avoidLabel("[1] Avoid Specific Ports", font, 16);
    avoidLabel.setPosition(400, startY + line * spacing);
    avoidLabel.setFillColor(Color(150, 150, 150));
    window.draw(avoidLabel);
    
    if (useAvoidPorts && !avoidPorts.empty()) {
        string portsList = "";
        for (int i = 0; i < avoidPorts.getSize(); i++) {
            portsList += avoidPorts[i];
            if (i < avoidPorts.getSize() - 1) portsList += ", ";
        }
        Text portsText(portsList, font, 14);
        portsText.setPosition(650, startY + line * spacing + 2);
        portsText.setFillColor(Color(0, 255, 100));
        window.draw(portsText);
    }
    line++;
    line++;
    
    // [2] Max voyage time
    Text timeLabel("[2] Maximum Voyage Time", font, 16);
    timeLabel.setPosition(400, startY + line * spacing);
    timeLabel.setFillColor(Color(150, 150, 150));
    window.draw(timeLabel);
    
    if (useMaxVoyageTime && maxVoyageTime > 0) {
        string timeStr = to_string((int)maxVoyageTime) + " hours (" 
                        + to_string((int)(maxVoyageTime / 24)) + " days)";
        Text timeText(timeStr, font, 14);
        timeText.setPosition(650, startY + line * spacing + 2);
        timeText.setFillColor(Color(0, 255, 100));
        window.draw(timeText);
    }
    line++;
    line++;
    line++;
    
    // [3] Continue
    Text continueLabel("[3] Continue", font, 16);
    continueLabel.setPosition(400, startY + line * spacing);
    continueLabel.setFillColor(Color(100, 255, 100));
    window.draw(continueLabel);
    
    // Footer
    Text footer("Press number to select", font, 14);
    FloatRect footerBounds = footer.getLocalBounds();
    footer.setOrigin(footerBounds.width / 2, 0);
    footer.setPosition(800, 720);
    footer.setFillColor(Color(100, 100, 100));
    window.draw(footer);
}

// Draw HUD panel - ENHANCED for better visibility
void Graphics::drawHUD(int sourceIdx, int destIdx, PathResult& result, Graph& graph, string state, string algorithm) {
    // HUD Background Panel - LARGER and MORE VISIBLE
    RectangleShape hudBg(Vector2f(350, 200));
    hudBg.setPosition(10, 900 - 210);  // Bottom left
    hudBg.setFillColor(Color(5, 10, 25, 250));
    hudBg.setOutlineColor(CYBER_CYAN);
    hudBg.setOutlineThickness(3);
    window.draw(hudBg);
    
    // Corner decorations for futuristic look
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner % 2 == 0) ? 12 : 356;
        float cy = (corner < 2) ? 692 : 896;
        
        RectangleShape accent1(Vector2f(15, 2));
        accent1.setPosition(cx + (corner % 2 == 0 ? 0 : -15), cy);
        accent1.setFillColor(CYBER_CYAN);
        window.draw(accent1);
        
        RectangleShape accent2(Vector2f(2, 15));
        accent2.setPosition(cx, cy + (corner < 2 ? 0 : -15));
        accent2.setFillColor(CYBER_CYAN);
        window.draw(accent2);
    }
    
    float startX = 20;
    float startY = 900 - 205;
    
    // Title - LARGER
    Text title("OCEANROUTE NAV", font, 14);
    title.setPosition(startX, startY);
    title.setFillColor(CYBER_CYAN);
    title.setStyle(Text::Bold);
    window.draw(title);
    
    // State indicator - MORE VISIBLE with background
    RectangleShape stateBg(Vector2f(155, 22));
    stateBg.setPosition(startX + 165, startY - 2);
    
    Text stateText(state, font, 11);
    stateText.setPosition(startX + 170, startY + 2);
    stateText.setStyle(Text::Bold);
    
    if (state == "IDLE" || state == "READY TO COMPUTE") {
        stateBg.setFillColor(Color(50, 50, 50, 200));
        stateText.setFillColor(Color(150, 150, 150));
    } else if (state.find("SELECTING") != string::npos || state.find("ENTER") != string::npos || state.find("INPUTTING") != string::npos) {
        stateBg.setFillColor(Color(80, 60, 0, 220));
        stateText.setFillColor(Color(255, 200, 0));
    } else if (state == "COMPUTING...") {
        stateBg.setFillColor(Color(80, 40, 0, 220));
        stateText.setFillColor(Color(255, 140, 0));
    } else if (state == "PATH FOUND") {
        stateBg.setFillColor(Color(0, 60, 0, 220));
        stateText.setFillColor(Color(0, 255, 100));
    } else {
        stateBg.setFillColor(Color(50, 50, 50, 200));
        stateText.setFillColor(Color(200, 200, 200));
    }
    
    window.draw(stateBg);
    window.draw(stateText);
    
    // Algorithm name with icon
    Text algoText("◉ " + algorithm, font, 12);
    algoText.setPosition(startX, startY + 22);
    algoText.setFillColor(Color(255, 215, 0));
    algoText.setStyle(Text::Bold);
    window.draw(algoText);
    
    // Separator line - THICKER
    RectangleShape line(Vector2f(330, 2));
    line.setPosition(startX, startY + 42);
    line.setFillColor(CYBER_CYAN);
    window.draw(line);
    
    // Source & Destination - LARGER and COLOR-CODED
    if (sourceIdx != -1) {
        Text srcLabel("► SOURCE:", font, 11);
        srcLabel.setPosition(startX, startY + 52);
        srcLabel.setFillColor(Color(100, 255, 100));
        srcLabel.setStyle(Text::Bold);
        window.draw(srcLabel);
        
        Text srcText(graph.ports[sourceIdx]->name, font, 12);
        srcText.setPosition(startX + 85, startY + 51);
        srcText.setFillColor(Color(0, 255, 0));
        srcText.setStyle(Text::Bold);
        window.draw(srcText);
    }
    
    if (destIdx != -1) {
        Text destLabel("► DESTINATION:", font, 11);
        destLabel.setPosition(startX, startY + 72);
        destLabel.setFillColor(Color(255, 150, 150));
        destLabel.setStyle(Text::Bold);
        window.draw(destLabel);
        
        Text destText(graph.ports[destIdx]->name, font, 12);
        destText.setPosition(startX + 115, startY + 71);
        destText.setFillColor(Color(255, 100, 100));
        destText.setStyle(Text::Bold);
        window.draw(destText);
    }
    
    // Results (if path computed) - ENHANCED
    if (result.pathFound) {
        RectangleShape separator(Vector2f(330, 2));
        separator.setPosition(startX, startY + 92);
        separator.setFillColor(Color(80, 80, 80));
        window.draw(separator);
        
        // Cost with icon
        Text costText("💰 Cost: $" + to_string((int)result.totalCost), font, 13);
        costText.setPosition(startX, startY + 102);
        costText.setFillColor(Color(255, 215, 0));
        costText.setStyle(Text::Bold);
        window.draw(costText);
        
        // Time with icon
        int days = (int)(result.totalTime / 24);
        int hours = (int)result.totalTime % 24;
        string timeStr = to_string(days) + "d " + to_string(hours) + "h";
        Text timeText("⏱ Time: " + timeStr, font, 12);
        timeText.setPosition(startX, startY + 124);
        timeText.setFillColor(Color(150, 200, 255));
        window.draw(timeText);
        
        // Hops count
        int hops = 0;
        Node<Route>* counter = result.routes.head;
        while (counter) {
            hops++;
            counter = counter->next;
        }
        
        // Show different stats for DFS
        if (algorithm.find("DFS") != string::npos) {
            Text statsText("🔍 Multiple Paths | Hops: " + to_string(hops), font, 11);
            statsText.setPosition(startX, startY + 146);
            statsText.setFillColor(Color(100, 200, 255));
            window.draw(statsText);
        } else {
            Text statsText("🔍 Explored: " + to_string(result.nodesExplored) + " | Hops: " + to_string(hops), font, 11);
            statsText.setPosition(startX, startY + 146);
            statsText.setFillColor(Color(100, 200, 255));
            window.draw(statsText);
        }
        
        // Efficiency indicator
        float efficiency = (result.nodesExplored > 0) ? (float)graph.ports.getSize() / result.nodesExplored : 0;
        Text effText("⚡ Efficiency: " + to_string((int)(efficiency * 100)) + "%", font, 11);
        effText.setPosition(startX, startY + 166);
        effText.setFillColor(efficiency > 0.5f ? Color(0, 255, 100) : Color(255, 200, 0));
        window.draw(effText);
    }
}

void Graphics::drawTraversalHUD(Graph& graph, PathResult& pathResult) {
    // Only show if ship is actively traversing
    if (!shipState.active || shipAnimations.empty()) return;
    
    int currentSeg = shipState.currentSegmentIndex;
    if (currentSeg < 0 || currentSeg >= shipAnimations.getSize()) return;
    
    RouteAnimation& currentRoute = shipAnimations[currentSeg];
    
    // Get the actual route from pathResult
    Node<Route>* routeNode = pathResult.routes.head;
    int segmentCount = 0;
    Route* actualRoute = nullptr;
    
    while (routeNode != nullptr) {
        if (segmentCount == currentSeg) {
            actualRoute = &routeNode->data;
            break;
        }
        segmentCount++;
        routeNode = routeNode->next;
    }
    
    if (actualRoute == nullptr) return;  // Safety check
    
    // Get port info
    int sourceIdx = currentRoute.sourceIdx;
    int destIdx = currentRoute.destIdx;
    
    if (sourceIdx < 0 || sourceIdx >= graph.ports.getSize() || 
        destIdx < 0 || destIdx >= graph.ports.getSize()) return;
    
    Port* sourcePort = graph.ports[sourceIdx];
    Port* destPort = graph.ports[destIdx];
    
    // HUD Panel - Top Right Corner
    float hudWidth = 380;
    float hudHeight = 260;
    float hudX = 1600 - hudWidth - 20;
    float hudY = 20;
    
    // Main panel background with gradient effect
    RectangleShape hudBg(Vector2f(hudWidth, hudHeight));
    hudBg.setPosition(hudX, hudY);
    hudBg.setFillColor(Color(10, 15, 30, 240));
    hudBg.setOutlineColor(Color(0, 200, 255));
    hudBg.setOutlineThickness(2);
    window.draw(hudBg);
    
    // Corner accents
    for (int i = 0; i < 4; i++) {
        float cx = (i % 2 == 0) ? hudX : hudX + hudWidth;
        float cy = (i < 2) ? hudY : hudY + hudHeight;
        
        RectangleShape corner1(Vector2f(20, 2));
        corner1.setPosition(cx + (i % 2 == 0 ? 2 : -22), cy + (i < 2 ? 2 : -2));
        corner1.setFillColor(Color(0, 200, 255));
        window.draw(corner1);
        
        RectangleShape corner2(Vector2f(2, 20));
        corner2.setPosition(cx + (i % 2 == 0 ? 2 : -2), cy + (i < 2 ? 2 : -22));
        corner2.setFillColor(Color(0, 200, 255));
        window.draw(corner2);
    }
    
    float contentX = hudX + 15;
    float contentY = hudY + 15;
    
    // Title with animation
    static Clock titleClock;
    float titleTime = titleClock.getElapsedTime().asSeconds();
    int titleAlpha = 200 + 55 * sin(titleTime * 2.0f);
    
    Text title("⚓ ROUTE TRAVERSAL", font, 16);
    title.setPosition(contentX, contentY);
    title.setFillColor(Color(0, 200, 255, titleAlpha));
    title.setStyle(Text::Bold);
    window.draw(title);
    
    // Progress indicator
    float progressBarWidth = hudWidth - 30;
    float progress = shipState.segmentProgress;
    
    RectangleShape progressBg(Vector2f(progressBarWidth, 8));
    progressBg.setPosition(contentX, contentY + 25);
    progressBg.setFillColor(Color(30, 30, 50, 200));
    progressBg.setOutlineColor(Color(80, 80, 100));
    progressBg.setOutlineThickness(1);
    window.draw(progressBg);
    
    RectangleShape progressFill(Vector2f(progressBarWidth * progress, 8));
    progressFill.setPosition(contentX, contentY + 25);
    progressFill.setFillColor(Color(0, 255, 150));
    window.draw(progressFill);
    
    // Segment counter
    Text segmentCounter("Leg " + to_string(currentSeg + 1) + "/" + to_string(shipAnimations.getSize()), font, 12);
    segmentCounter.setPosition(contentX + progressBarWidth - 60, contentY + 28);
    segmentCounter.setFillColor(Color(255, 255, 255));
    segmentCounter.setStyle(Text::Bold);
    window.draw(segmentCounter);
    
    contentY += 45;
    
    // Current Route Section Header
    RectangleShape sectionHeader(Vector2f(hudWidth - 30, 22));
    sectionHeader.setPosition(contentX, contentY);
    sectionHeader.setFillColor(Color(0, 100, 150, 150));
    window.draw(sectionHeader);
    
    Text sectionTitle("CURRENT VOYAGE", font, 12);
    sectionTitle.setPosition(contentX + 5, contentY + 4);
    sectionTitle.setFillColor(Color(255, 255, 255));
    sectionTitle.setStyle(Text::Bold);
    window.draw(sectionTitle);
    
    contentY += 28;
    
    // Route: Source → Destination
    Text routeLabel("Route:", font, 11);
    routeLabel.setPosition(contentX, contentY);
    routeLabel.setFillColor(Color(180, 180, 180));
    window.draw(routeLabel);
    
    Text routeText(sourcePort->name + " → " + destPort->name, font, 12);
    routeText.setPosition(contentX + 50, contentY - 1);
    routeText.setFillColor(Color(0, 255, 150));
    routeText.setStyle(Text::Bold);
    window.draw(routeText);
    
    contentY += 22;
    
    // Company
    Text companyLabel("Company:", font, 11);
    companyLabel.setPosition(contentX, contentY);
    companyLabel.setFillColor(Color(180, 180, 180));
    window.draw(companyLabel);
    
    Text companyText(currentRoute.company, font, 11);
    companyText.setPosition(contentX + 75, contentY);
    companyText.setFillColor(Color(255, 215, 0));
    window.draw(companyText);
    
    contentY += 22;
    
    // Departure Time
    Text depLabel("Depart:", font, 11);
    depLabel.setPosition(contentX, contentY);
    depLabel.setFillColor(Color(180, 180, 180));
    window.draw(depLabel);
    
    string depTime = to_string(currentRoute.time.day) + "/" + 
                    to_string(currentRoute.time.month) + "/" + 
                    to_string(currentRoute.time.year) + " " +
                    to_string(currentRoute.time.hour) + ":" + 
                    (currentRoute.time.minute < 10 ? "0" : "") + to_string(currentRoute.time.minute);
    
    Text depText(depTime, font, 11);
    depText.setPosition(contentX + 75, contentY);
    depText.setFillColor(Color(100, 255, 100));
    window.draw(depText);
    
    contentY += 22;
    
    // Calculate arrival time (need to get it from shipAnimations route data)
    // For now, show voyage cost
    Text costLabel("Cost:", font, 11);
    costLabel.setPosition(contentX, contentY);
    costLabel.setFillColor(Color(180, 180, 180));
    window.draw(costLabel);
    
    Text costText("$" + to_string((int)currentRoute.cost), font, 11);
    costText.setPosition(contentX + 75, contentY);
    costText.setFillColor(Color(255, 200, 0));
    costText.setStyle(Text::Bold);
    window.draw(costText);
    
    contentY += 28;
    
    // Layover Information Section
    RectangleShape layoverHeader(Vector2f(hudWidth - 30, 22));
    layoverHeader.setPosition(contentX, contentY);
    layoverHeader.setFillColor(Color(100, 50, 0, 150));
    window.draw(layoverHeader);
    
    Text layoverTitle("DOCKING & PORT INFO", font, 12);
    layoverTitle.setPosition(contentX + 5, contentY + 4);
    layoverTitle.setFillColor(Color(255, 255, 255));
    layoverTitle.setStyle(Text::Bold);
    window.draw(layoverTitle);
    
    contentY += 28;
    
    // Use the backend freeTime function with actual route data from PathResult
    Ship currentShip;
    currentShip.arrivalTime = actualRoute->arrivalTime;
    currentShip.companyName = actualRoute->company;
    currentShip.destinationPort = actualRoute->destinationPortName;
    currentShip.sourcePort = actualRoute->sourcePortName;
    
    // freeTime calculates when this ship can leave (wait + 2h docking)
    double dockingTime = graph.freeTime(destPort, currentShip);
    
    Text dockingLabel("Docking Time:", font, 11);
    dockingLabel.setPosition(contentX, contentY);
    dockingLabel.setFillColor(Color(180, 180, 180));
    window.draw(dockingLabel);
    
    // Convert to hours and minutes
    int hours = (int)dockingTime;
    int minutes = (int)((dockingTime - hours) * 60);
    string timeStr = to_string(hours) + "h " + to_string(minutes) + "m";
    
    Text dockingText(timeStr, font, 11);
    dockingText.setPosition(contentX + 95, contentY);
    dockingText.setFillColor((dockingTime > 12.0) ? Color(255, 150, 0) : Color(0, 255, 100));
    window.draw(dockingText);
    
    contentY += 22;
    
    // Port charges at destination - ONLY if docking time > 12 hours
    if (dockingTime > 12.0) {
        Text chargesLabel("Port Fee:", font, 11);
        chargesLabel.setPosition(contentX, contentY);
        chargesLabel.setFillColor(Color(180, 180, 180));
        window.draw(chargesLabel);
        
        // Calculate layover fee based on docking time (same as backend logic)
        double layoverFee = destPort->portCharges * (dockingTime / 24.0);
        
        Text chargesText("$" + to_string((int)layoverFee) + " (" + to_string((int)dockingTime) + "h)", font, 11);
        chargesText.setPosition(contentX + 95, contentY);
        chargesText.setFillColor(Color(255, 100, 100));
        chargesText.setStyle(Text::Bold);
        window.draw(chargesText);
    } else {
        // Show "No fee" when docking time <= 12 hours
        Text chargesLabel("Port Fee:", font, 11);
        chargesLabel.setPosition(contentX, contentY);
        chargesLabel.setFillColor(Color(180, 180, 180));
        window.draw(chargesLabel);
        
        Text chargesText("No fee (<12h)", font, 11);
        chargesText.setPosition(contentX + 95, contentY);
        chargesText.setFillColor(Color(0, 200, 100));
        window.draw(chargesText);
    }
    
    // Add pulsing animation to current segment indicator
    static Clock pulseClock;
    float pulseTime = pulseClock.getElapsedTime().asSeconds();
    int pulseAlpha = 150 + 105 * sin(pulseTime * 4.0f);
    
    CircleShape pulseIndicator(8);
    pulseIndicator.setPosition(contentX - 18, hudY + 15);
    pulseIndicator.setFillColor(Color(0, 255, 150, pulseAlpha));
    window.draw(pulseIndicator);
}


void Graphics::setExplorationData(const PathResult& result) {
    // No more exploration history - ship IS the exploration
    completedSegments.clear();
}

void Graphics::startShipAnimation(const LinkedList<Route>& path, Graph& graph) {
    shipAnimations.clear();
    completedSegments.clear();
    
    // Create ship animations for each route segment
    Node<Route>* current = path.head;
    
    while (current != nullptr) {
        int sourceIdx = graph.findPortIndex(current->data.sourcePortName);
        if (sourceIdx == -1) {
            current = current->next;
            continue;
        }
        
        RouteAnimation ship;
        ship.sourceIdx = sourceIdx;
        ship.destIdx = current->data.destinationIndex;
        ship.progress = 0.0f;
        ship.speed = 0.2f;  // Slower for clarity
        ship.color = getCompanyColor(current->data.company);
        ship.company = current->data.company;
        ship.cost = current->data.voyageCost;
        ship.time = current->data.departureTime;
        ship.arrivalTime = current->data.arrivalTime;
        
        shipAnimations.push_back(ship);
        current = current->next;
    }
    
    // Start ship journey
    if (!shipAnimations.empty()) {
        shipState.active = true;
        shipState.currentSegmentIndex = 0;
        shipState.segmentProgress = 0.0f;
    }
}

// Helper to get port continent (forward declaration from main.cpp)
extern string getPortContinent(const string& portName);

void Graphics::drawGraphAnalysis(Graph& graph, bool showFilterPanel,
                                 Vector<string>& continentFilters, Vector<bool>& continentActive,
                                 Vector<string>& companyFilters, Vector<bool>& companyActive,
                                 int hoveredPortIdx, int hoveredRouteSourceIdx, int hoveredRouteDestIdx,
                                 Route& hoveredRoute) {
    // Draw map background
    window.draw(mapSprite);
    
    // Check if any filters are active
    bool anyContinentActive = false;
    for (int i = 0; i < continentActive.getSize(); i++) {
        if (continentActive[i]) {
            anyContinentActive = true;
            break;
        }
    }
    
    bool anyCompanyActive = false;
    for (int i = 0; i < companyActive.getSize(); i++) {
        if (companyActive[i]) {
            anyCompanyActive = true;
            break;
        }
    }
    
    bool anyFilterActive = anyContinentActive || anyCompanyActive;
    
    // Draw all routes/edges with filtering
    for (int i = 0; i < graph.ports.getSize(); i++) {
        Port* source = graph.ports[i];
        Node<Route>* current = source->routes.head;
        
        while (current != nullptr) {
            Route& route = current->data;
            int destIdx = graph.findPortIndex(route.destinationPortName);
            
            if (destIdx != -1) {
                Vector2f sourcePos = portScreenPositions[i];
                Vector2f destPos = portScreenPositions[destIdx];
                
                Color routeColor;
                
                if (!anyFilterActive) {
                    // No filters active - show normal routes
                    routeColor = getCompanyColor(route.company);
                    routeColor.a = 120;  // Semi-transparent
                } else {
                    // Filters active - apply filtering logic
                    // Check company filter
                    bool companyMatch = !anyCompanyActive;
                    if (anyCompanyActive) {
                        for (int c = 0; c < companyFilters.getSize(); c++) {
                            if (companyActive[c] && route.company == companyFilters[c]) {
                                companyMatch = true;
                                break;
                            }
                        }
                    }
                    
                    // Check continent filter
                    bool continentMatch = !anyContinentActive;
                    if (anyContinentActive) {
                        string sourceCont = getPortContinent(source->name);
                        string destCont = getPortContinent(route.destinationPortName);
                        
                        for (int c = 0; c < continentFilters.getSize(); c++) {
                            if (continentActive[c] && 
                                (sourceCont == continentFilters[c] || destCont == continentFilters[c])) {
                                continentMatch = true;
                                break;
                            }
                        }
                    }
                    
                    // Apply colors based on match
                    if (companyMatch && continentMatch) {
                        routeColor = Color(255, 215, 0, 200);  // Golden glow
                    } else {
                        routeColor = Color(100, 100, 100, 50);  // Faded
                    }
                }
                
                // Draw route line
                Vertex line[] = {
                    Vertex(sourcePos, routeColor),
                    Vertex(destPos, routeColor)
                };
                window.draw(line, 2, Lines);
            }
            
            current = current->next;
        }
    }
    
    // Draw ports with filtering
    for (int i = 0; i < graph.ports.getSize(); i++) {
        Port* port = graph.ports[i];
        Vector2f pos = portScreenPositions[i];
        bool isHovered = (i == hoveredPortIdx);
        
        if (!anyFilterActive) {
            // No filters active - show normal ports
            CircleShape portCircle(isHovered ? 8.0f : 6.0f);
            portCircle.setOrigin(isHovered ? 8.0f : 6.0f, isHovered ? 8.0f : 6.0f);
            portCircle.setPosition(pos);
            portCircle.setFillColor(Color(0, 200, 255, 200));
            portCircle.setOutlineColor(Color(0, 255, 255, 255));
            portCircle.setOutlineThickness(isHovered ? 2 : 1);
            
            // Glow effect when hovered
            if (isHovered) {
                for (int g = 1; g <= 3; g++) {
                    CircleShape glow(8.0f + g * 3);
                    glow.setOrigin(8.0f + g * 3, 8.0f + g * 3);
                    glow.setPosition(pos);
                    glow.setFillColor(Color(0, 200, 255, 60 / g));
                    window.draw(glow);
                }
            }
            
            window.draw(portCircle);
            
            // Only show name when hovered
            if (isHovered) {
                drawPortTooltip(port, pos, graph);
            }
        } else {
            // Filters active - apply filtering
            bool portMatch = !anyContinentActive;
            if (anyContinentActive) {
                string portCont = getPortContinent(port->name);
                for (int c = 0; c < continentFilters.getSize(); c++) {
                    if (continentActive[c] && portCont == continentFilters[c]) {
                        portMatch = true;
                        break;
                    }
                }
            }
            
            // Draw port circle
            float radius = portMatch ? 8.0f : 5.0f;
            if (isHovered) radius += 2.0f;
            
            CircleShape portCircle(radius);
            portCircle.setOrigin(radius, radius);
            portCircle.setPosition(pos);
            
            if (portMatch) {
                // Golden glow for filtered ports
                portCircle.setFillColor(Color(255, 215, 0, 220));
                portCircle.setOutlineColor(Color(255, 223, 0, 255));
                portCircle.setOutlineThickness(isHovered ? 3 : 2);
                
                // Draw glow effect
                for (int g = 1; g <= 3; g++) {
                    CircleShape glow(radius + g * 3);
                    glow.setOrigin(radius + g * 3, radius + g * 3);
                    glow.setPosition(pos);
                    glow.setFillColor(Color(255, 215, 0, 60 / g));
                    window.draw(glow);
                }
            } else {
                // Faded for non-matching ports
                portCircle.setFillColor(Color(150, 150, 150, 100));
                portCircle.setOutlineColor(Color(100, 100, 100, 80));
                portCircle.setOutlineThickness(isHovered ? 2 : 1);
            }
            
            window.draw(portCircle);
            
            // Only show tooltip when hovered
            if (isHovered) {
                drawPortTooltip(port, pos, graph);
            }
        }
    }
    
    // Draw route tooltip if hovering over a route
    if (hoveredRouteSourceIdx != -1 && hoveredRouteDestIdx != -1) {
        Vector2i mousePos = Mouse::getPosition(window);
        string sourceName = graph.ports[hoveredRouteSourceIdx]->name;
        string destName = graph.ports[hoveredRouteDestIdx]->name;
        drawRouteTooltip(hoveredRoute, Vector2f(mousePos.x, mousePos.y), sourceName, destName);
    }
    
    // Draw filter button
    drawFilterButton(showFilterPanel);
    
    // Draw filter panel if open
    if (showFilterPanel) {
        drawFilterPanel(continentFilters, continentActive, companyFilters, companyActive);
    }
    
    // Draw instructions
    Text instructions("GRAPH ANALYSIS MODE | Click Filter Button to Toggle Filters | [ESC] Back to Menu", font, 14);
    FloatRect bounds = instructions.getLocalBounds();
    instructions.setOrigin(bounds.width / 2, 0);
    instructions.setPosition(800, 20);
    instructions.setFillColor(Color(0, 255, 255));
    window.draw(instructions);
}

void Graphics::drawFilterButton(bool panelOpen) {
    // Filter button in bottom left
    RectangleShape button(Vector2f(120, 40));
    button.setPosition(20, 820);
    button.setFillColor(panelOpen ? Color(0, 80, 120, 220) : Color(10, 25, 40, 200));
    button.setOutlineColor(Color(0, 255, 255));
    button.setOutlineThickness(2);
    window.draw(button);
    
    Text buttonText("FILTERS", font, 16);
    buttonText.setPosition(45, 830);
    buttonText.setFillColor(Color(0, 255, 255));
    buttonText.setStyle(Text::Bold);
    window.draw(buttonText);
}

void Graphics::drawFilterPanel(Vector<string>& continentFilters, Vector<bool>& continentActive,
                               Vector<string>& companyFilters, Vector<bool>& companyActive) {
    // Center the panel
    float panelWidth = 600;
    float filterHeight = 35;
    float filterSpacing = 8;
    float sectionSpacing = 40;
    
    // Calculate panel height
    float panelHeight = 120 + continentFilters.getSize() * (filterHeight + filterSpacing) + 
                        sectionSpacing + companyFilters.getSize() * (filterHeight + filterSpacing);
    
    float panelX = (1600 - panelWidth) / 2;  // Center horizontally
    float panelY = (900 - panelHeight) / 2;   // Center vertically
    
    // Draw molecular network background
    static float moleculePhase = 0.0f;
    moleculePhase += 0.01f;
    
    // Dark overlay for contrast
    RectangleShape darkOverlay(Vector2f(1600, 900));
    darkOverlay.setFillColor(Color(0, 0, 0, 180));
    window.draw(darkOverlay);
    
    // Draw molecular particles
    for (int i = 0; i < 40; i++) {
        float angle = (i / 40.0f) * 6.28f + moleculePhase;
        float radius = 150 + 50 * sin(moleculePhase + i * 0.5f);
        float x = 800 + cos(angle) * radius;
        float y = 450 + sin(angle) * radius;
        
        CircleShape molecule(3);
        molecule.setPosition(x, y);
        molecule.setFillColor(Color(0, 200, 255, 150));
        window.draw(molecule);
        
        // Connect nearby molecules
        for (int j = i + 1; j < 40; j++) {
            float angle2 = (j / 40.0f) * 6.28f + moleculePhase;
            float radius2 = 150 + 50 * sin(moleculePhase + j * 0.5f);
            float x2 = 800 + cos(angle2) * radius2;
            float y2 = 450 + sin(angle2) * radius2;
            
            float dist = sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
            if (dist < 100) {
                Vertex line[] = {
                    Vertex(Vector2f(x, y), Color(0, 200, 255, 50)),
                    Vertex(Vector2f(x2, y2), Color(0, 200, 255, 50))
                };
                window.draw(line, 2, Lines);
            }
        }
    }
    
    // Main panel background with glow
    for (int glow = 3; glow > 0; glow--) {
        RectangleShape glowBg(Vector2f(panelWidth + glow * 4, panelHeight + glow * 4));
        glowBg.setPosition(panelX - glow * 2, panelY - glow * 2);
        glowBg.setFillColor(Color(0, 150, 200, 30 / glow));
        window.draw(glowBg);
    }
    
    // Panel background
    RectangleShape panelBg(Vector2f(panelWidth, panelHeight));
    panelBg.setPosition(panelX, panelY);
    panelBg.setFillColor(Color(5, 15, 30, 250));
    panelBg.setOutlineColor(Color(0, 255, 255, 255));
    panelBg.setOutlineThickness(3);
    window.draw(panelBg);
    
    // Corner accents
    float cornerSize = 20;
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner % 2 == 0) ? panelX : panelX + panelWidth;
        float cy = (corner < 2) ? panelY : panelY + panelHeight;
        
        RectangleShape accent1(Vector2f(cornerSize, 3));
        accent1.setPosition(cx + (corner % 2 == 0 ? 0 : -cornerSize), cy);
        accent1.setFillColor(Color(0, 255, 255, 255));
        window.draw(accent1);
        
        RectangleShape accent2(Vector2f(3, cornerSize));
        accent2.setPosition(cx, cy + (corner < 2 ? 0 : -cornerSize));
        accent2.setFillColor(Color(0, 255, 255, 255));
        window.draw(accent2);
    }
    
    // Title
    Text title("FILTER CONFIGURATION", font, 28);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, 0);
    title.setPosition(800, panelY + 20);
    title.setFillColor(Color(0, 255, 255));
    title.setStyle(Text::Bold);
    window.draw(title);
    
    // Animated underline
    RectangleShape underline(Vector2f(titleBounds.width + 40, 2));
    underline.setPosition(800 - titleBounds.width / 2 - 20, panelY + 55);
    underline.setFillColor(Color(0, 255, 255, 200));
    window.draw(underline);
    
    float contentY = panelY + 80;
    
    // Continent section
    Text continentTitle("CONTINENT FILTERS", font, 16);
    continentTitle.setPosition(panelX + 30, contentY);
    continentTitle.setFillColor(Color(100, 200, 255));
    continentTitle.setStyle(Text::Bold);
    window.draw(continentTitle);
    
    // Continent filters in 2 columns
    float col1X = panelX + 30;
    float col2X = panelX + 320;
    int filtersPerCol = (continentFilters.getSize() + 1) / 2;
    
    for (int i = 0; i < continentFilters.getSize(); i++) {
        float x = (i < filtersPerCol) ? col1X : col2X;
        float y = contentY + 35 + (i % filtersPerCol) * (filterHeight + filterSpacing);
        
        // Hexagonal-style button
        RectangleShape filterBox(Vector2f(250, filterHeight));
        filterBox.setPosition(x, y);
        
        if (continentActive[i]) {
            filterBox.setFillColor(Color(255, 215, 0, 180));
            filterBox.setOutlineColor(Color(255, 223, 0, 255));
            filterBox.setOutlineThickness(2);
            
            // Glow effect for active
            RectangleShape glow(Vector2f(254, filterHeight + 4));
            glow.setPosition(x - 2, y - 2);
            glow.setFillColor(Color::Transparent);
            glow.setOutlineColor(Color(255, 215, 0, 100));
            glow.setOutlineThickness(2);
            window.draw(glow);
        } else {
            filterBox.setFillColor(Color(15, 25, 45, 220));
            filterBox.setOutlineColor(Color(0, 150, 200, 180));
            filterBox.setOutlineThickness(1);
        }
        
        window.draw(filterBox);
        
        // Side accent
        RectangleShape accent(Vector2f(3, filterHeight));
        accent.setPosition(x, y);
        accent.setFillColor(continentActive[i] ? Color(255, 215, 0) : Color(0, 200, 255, 150));
        window.draw(accent);
        
        // Filter text
        Text filterText(continentFilters[i], font, 14);
        filterText.setPosition(x + 15, y + 9);
        filterText.setFillColor(continentActive[i] ? Color(10, 10, 10) : Color(200, 220, 255));
        if (continentActive[i]) filterText.setStyle(Text::Bold);
        window.draw(filterText);
    }
    
    // Company section
    float companyY = contentY + 35 + filtersPerCol * (filterHeight + filterSpacing) + sectionSpacing;
    Text companyTitle("SHIPPING COMPANY FILTERS", font, 16);
    companyTitle.setPosition(panelX + 30, companyY);
    companyTitle.setFillColor(Color(100, 200, 255));
    companyTitle.setStyle(Text::Bold);
    window.draw(companyTitle);
    
    // Company filters in 2 columns
    int companyPerCol = (companyFilters.getSize() + 1) / 2;
    
    for (int i = 0; i < companyFilters.getSize(); i++) {
        float x = (i < companyPerCol) ? col1X : col2X;
        float y = companyY + 35 + (i % companyPerCol) * (filterHeight + filterSpacing);
        
        RectangleShape filterBox(Vector2f(250, filterHeight));
        filterBox.setPosition(x, y);
        
        if (companyActive[i]) {
            filterBox.setFillColor(Color(255, 215, 0, 180));
            filterBox.setOutlineColor(Color(255, 223, 0, 255));
            filterBox.setOutlineThickness(2);
            
            RectangleShape glow(Vector2f(254, filterHeight + 4));
            glow.setPosition(x - 2, y - 2);
            glow.setFillColor(Color::Transparent);
            glow.setOutlineColor(Color(255, 215, 0, 100));
            glow.setOutlineThickness(2);
            window.draw(glow);
        } else {
            filterBox.setFillColor(Color(15, 25, 45, 220));
            filterBox.setOutlineColor(Color(0, 150, 200, 180));
            filterBox.setOutlineThickness(1);
        }
        
        window.draw(filterBox);
        
        RectangleShape accent(Vector2f(3, filterHeight));
        accent.setPosition(x, y);
        accent.setFillColor(companyActive[i] ? Color(255, 215, 0) : Color(0, 200, 255, 150));
        window.draw(accent);
        
        Text filterText(companyFilters[i], font, 14);
        filterText.setPosition(x + 15, y + 9);
        filterText.setFillColor(companyActive[i] ? Color(10, 10, 10) : Color(200, 220, 255));
        if (companyActive[i]) filterText.setStyle(Text::Bold);
        window.draw(filterText);
    }
    
    // Bottom instructions
    Text instructions("Click filters to toggle | Multiple selections allowed | [ESC] to close", font, 13);
    FloatRect instrBounds = instructions.getLocalBounds();
    instructions.setOrigin(instrBounds.width / 2, 0);
    instructions.setPosition(800, panelY + panelHeight - 35);
    instructions.setFillColor(Color(100, 200, 255, 200));
    window.draw(instructions);
}

void Graphics::drawPortTooltip(Port* port, Vector2f pos, Graph& graph) {
    // Tooltip background dimensions
    float tooltipWidth = 280;
    float tooltipHeight = 150;
    float offsetX = 15;
    float offsetY = -tooltipHeight - 10;
    
    // Adjust position to keep tooltip on screen
    float tooltipX = pos.x + offsetX;
    float tooltipY = pos.y + offsetY;
    
    if (tooltipX + tooltipWidth > 1590) tooltipX = pos.x - tooltipWidth - 15;
    if (tooltipY < 10) tooltipY = pos.y + 20;
    
    // Tooltip background with glow
    RectangleShape tooltipGlow(Vector2f(tooltipWidth + 6, tooltipHeight + 6));
    tooltipGlow.setPosition(tooltipX - 3, tooltipY - 3);
    tooltipGlow.setFillColor(Color(0, 200, 255, 50));
    window.draw(tooltipGlow);
    
    RectangleShape tooltipBg(Vector2f(tooltipWidth, tooltipHeight));
    tooltipBg.setPosition(tooltipX, tooltipY);
    tooltipBg.setFillColor(Color(5, 15, 30, 250));
    tooltipBg.setOutlineColor(Color(0, 255, 255, 255));
    tooltipBg.setOutlineThickness(2);
    window.draw(tooltipBg);
    
    // Corner accents
    float cornerSize = 12;
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner % 2 == 0) ? tooltipX : tooltipX + tooltipWidth;
        float cy = (corner < 2) ? tooltipY : tooltipY + tooltipHeight;
        
        RectangleShape accent1(Vector2f(cornerSize, 2));
        accent1.setPosition(cx + (corner % 2 == 0 ? 0 : -cornerSize), cy);
        accent1.setFillColor(Color(0, 255, 255));
        window.draw(accent1);
        
        RectangleShape accent2(Vector2f(2, cornerSize));
        accent2.setPosition(cx, cy + (corner < 2 ? 0 : -cornerSize));
        accent2.setFillColor(Color(0, 255, 255));
        window.draw(accent2);
    }
    
    // Port name (title)
    Text nameText(port->name, font, 16);
    nameText.setPosition(tooltipX + 10, tooltipY + 10);
    nameText.setFillColor(Color(0, 255, 255));
    nameText.setStyle(Text::Bold);
    window.draw(nameText);
    
    // Separator line
    RectangleShape separator(Vector2f(tooltipWidth - 20, 2));
    separator.setPosition(tooltipX + 10, tooltipY + 35);
    separator.setFillColor(Color(0, 200, 255, 150));
    window.draw(separator);
    
    // Port charges
    Text chargesLabel("Port Charges:", font, 12);
    chargesLabel.setPosition(tooltipX + 10, tooltipY + 45);
    chargesLabel.setFillColor(Color(150, 200, 255));
    window.draw(chargesLabel);
    
    Text chargesValue("$" + to_string((int)port->portCharges), font, 12);
    chargesValue.setPosition(tooltipX + 150, tooltipY + 45);
    chargesValue.setFillColor(Color(255, 215, 0));
    chargesValue.setStyle(Text::Bold);
    window.draw(chargesValue);
    
    // Coordinates
    Text coordLabel("Coordinates:", font, 12);
    coordLabel.setPosition(tooltipX + 10, tooltipY + 65);
    coordLabel.setFillColor(Color(150, 200, 255));
    window.draw(coordLabel);
    
    char coordStr[50];
    sprintf(coordStr, "%.2f°, %.2f°", port->latitude, port->longitude);
    Text coordValue(coordStr, font, 11);
    coordValue.setPosition(tooltipX + 10, tooltipY + 82);
    coordValue.setFillColor(Color(200, 220, 255));
    window.draw(coordValue);
    
    // Number of routes
    Text routesLabel("Outbound Routes:", font, 12);
    routesLabel.setPosition(tooltipX + 10, tooltipY + 100);
    routesLabel.setFillColor(Color(150, 200, 255));
    window.draw(routesLabel);
    
    int routeCount = 0;
    Node<Route>* current = port->routes.head;
    while (current != nullptr) {
        routeCount++;
        current = current->next;
    }
    
    Text routesValue(to_string(routeCount), font, 12);
    routesValue.setPosition(tooltipX + 150, tooltipY + 100);
    routesValue.setFillColor(Color(0, 255, 200));
    routesValue.setStyle(Text::Bold);
    window.draw(routesValue);
    
    // Ships in queue
    Text queueLabel("Ships Docked:", font, 12);
    queueLabel.setPosition(tooltipX + 10, tooltipY + 120);
    queueLabel.setFillColor(Color(150, 200, 255));
    window.draw(queueLabel);
    
    // Count ships in queue manually
    int shipCount = 0;
    Queue<Ship> tempQueue = port->shipsQueue;
    while (!tempQueue.isEmpty()) {
        shipCount++;
        tempQueue.dequeue();
    }
    
    Text queueValue(to_string(shipCount), font, 12);
    queueValue.setPosition(tooltipX + 150, tooltipY + 120);
    queueValue.setFillColor(Color(255, 100, 100));
    queueValue.setStyle(Text::Bold);
    window.draw(queueValue);
}

void Graphics::drawRouteTooltip(Route& route, Vector2f mousePos, string sourceName, string destName) {
    float tooltipWidth = 300;
    float tooltipHeight = 180;
    float tooltipX = mousePos.x + 15;
    float tooltipY = mousePos.y - tooltipHeight / 2;
    
    // Keep tooltip on screen
    if (tooltipX + tooltipWidth > 1590) tooltipX = mousePos.x - tooltipWidth - 15;
    if (tooltipY < 10) tooltipY = 10;
    if (tooltipY + tooltipHeight > 890) tooltipY = 890 - tooltipHeight;
    
    // Tooltip background with glow
    RectangleShape tooltipGlow(Vector2f(tooltipWidth + 6, tooltipHeight + 6));
    tooltipGlow.setPosition(tooltipX - 3, tooltipY - 3);
    tooltipGlow.setFillColor(Color(255, 200, 0, 50));
    window.draw(tooltipGlow);
    
    RectangleShape tooltipBg(Vector2f(tooltipWidth, tooltipHeight));
    tooltipBg.setPosition(tooltipX, tooltipY);
    tooltipBg.setFillColor(Color(5, 15, 30, 250));
    tooltipBg.setOutlineColor(Color(255, 215, 0, 255));
    tooltipBg.setOutlineThickness(2);
    window.draw(tooltipBg);
    
    // Corner accents
    float cornerSize = 12;
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner % 2 == 0) ? tooltipX : tooltipX + tooltipWidth;
        float cy = (corner < 2) ? tooltipY : tooltipY + tooltipHeight;
        
        RectangleShape accent1(Vector2f(cornerSize, 2));
        accent1.setPosition(cx + (corner % 2 == 0 ? 0 : -cornerSize), cy);
        accent1.setFillColor(Color(255, 215, 0));
        window.draw(accent1);
        
        RectangleShape accent2(Vector2f(2, cornerSize));
        accent2.setPosition(cx, cy + (corner < 2 ? 0 : -cornerSize));
        accent2.setFillColor(Color(255, 215, 0));
        window.draw(accent2);
    }
    
    // Title
    Text title("ROUTE DETAILS", font, 14);
    title.setPosition(tooltipX + 10, tooltipY + 8);
    title.setFillColor(Color(255, 215, 0));
    title.setStyle(Text::Bold);
    window.draw(title);
    
    // Separator
    RectangleShape separator(Vector2f(tooltipWidth - 20, 2));
    separator.setPosition(tooltipX + 10, tooltipY + 30);
    separator.setFillColor(Color(255, 200, 0, 150));
    window.draw(separator);
    
    // Source -> Destination
    Text sourceLabel("From:", font, 11);
    sourceLabel.setPosition(tooltipX + 10, tooltipY + 40);
    sourceLabel.setFillColor(Color(150, 200, 255));
    window.draw(sourceLabel);
    
    Text sourceValue(sourceName, font, 11);
    sourceValue.setPosition(tooltipX + 80, tooltipY + 40);
    sourceValue.setFillColor(Color(255, 255, 255));
    sourceValue.setStyle(Text::Bold);
    window.draw(sourceValue);
    
    Text destLabel("To:", font, 11);
    destLabel.setPosition(tooltipX + 10, tooltipY + 58);
    destLabel.setFillColor(Color(150, 200, 255));
    window.draw(destLabel);
    
    Text destValue(destName, font, 11);
    destValue.setPosition(tooltipX + 80, tooltipY + 58);
    destValue.setFillColor(Color(255, 255, 255));
    destValue.setStyle(Text::Bold);
    window.draw(destValue);
    
    // Company
    Text companyLabel("Company:", font, 11);
    companyLabel.setPosition(tooltipX + 10, tooltipY + 80);
    companyLabel.setFillColor(Color(150, 200, 255));
    window.draw(companyLabel);
    
    Text companyValue(route.company, font, 11);
    companyValue.setPosition(tooltipX + 80, tooltipY + 80);
    companyValue.setFillColor(getCompanyColor(route.company));
    companyValue.setStyle(Text::Bold);
    window.draw(companyValue);
    
    // Cost
    Text costLabel("Voyage Cost:", font, 11);
    costLabel.setPosition(tooltipX + 10, tooltipY + 100);
    costLabel.setFillColor(Color(150, 200, 255));
    window.draw(costLabel);
    
    Text costValue("$" + to_string((int)route.voyageCost), font, 11);
    costValue.setPosition(tooltipX + 110, tooltipY + 100);
    costValue.setFillColor(Color(0, 255, 100));
    costValue.setStyle(Text::Bold);
    window.draw(costValue);
    
    // Departure time
    Text depLabel("Departure:", font, 11);
    depLabel.setPosition(tooltipX + 10, tooltipY + 120);
    depLabel.setFillColor(Color(150, 200, 255));
    window.draw(depLabel);
    
    char depStr[50];
    sprintf(depStr, "%02d:%02d %02d/%02d/%04d", 
            route.departureTime.hour, route.departureTime.minute,
            route.departureTime.day, route.departureTime.month, route.departureTime.year);
    Text depValue(depStr, font, 10);
    depValue.setPosition(tooltipX + 110, tooltipY + 120);
    depValue.setFillColor(Color(200, 220, 255));
    window.draw(depValue);
    
    // Arrival time
    Text arrLabel("Arrival:", font, 11);
    arrLabel.setPosition(tooltipX + 10, tooltipY + 140);
    arrLabel.setFillColor(Color(150, 200, 255));
    window.draw(arrLabel);
    
    char arrStr[50];
    sprintf(arrStr, "%02d:%02d %02d/%02d/%04d", 
            route.arrivalTime.hour, route.arrivalTime.minute,
            route.arrivalTime.day, route.arrivalTime.month, route.arrivalTime.year);
    Text arrValue(arrStr, font, 10);
    arrValue.setPosition(tooltipX + 110, tooltipY + 140);
    arrValue.setFillColor(Color(200, 220, 255));
    window.draw(arrValue);
    
    // Duration (calculate manually)
    double durationHours = 0;
    
    // Calculate hours difference
    int totalMinutesArr = route.arrivalTime.year * 525600 + route.arrivalTime.month * 43800 + 
                          route.arrivalTime.day * 1440 + route.arrivalTime.hour * 60 + route.arrivalTime.minute;
    int totalMinutesDep = route.departureTime.year * 525600 + route.departureTime.month * 43800 + 
                          route.departureTime.day * 1440 + route.departureTime.hour * 60 + route.departureTime.minute;
    durationHours = (totalMinutesArr - totalMinutesDep) / 60.0;
    
    Text durationLabel("Duration:", font, 11);
    durationLabel.setPosition(tooltipX + 10, tooltipY + 160);
    durationLabel.setFillColor(Color(150, 200, 255));
    window.draw(durationLabel);
    
    char durStr[30];
    sprintf(durStr, "%.1f hours", durationHours);
    Text durationValue(durStr, font, 11);
    durationValue.setPosition(tooltipX + 110, tooltipY + 160);
    durationValue.setFillColor(Color(255, 200, 100));
    durationValue.setStyle(Text::Bold);
    window.draw(durationValue);
}

