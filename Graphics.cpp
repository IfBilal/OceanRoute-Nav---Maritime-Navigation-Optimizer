#include "Graphics.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// --- Helper Functions ---
// RE-CALIBRATED COORDINATES FOR YOUR SPECIFIC MAP
sf::Vector2f Graphics::getRelativeCoordinates(string city) {
    // --- Americas ---
    if (city == "Vancouver")   return sf::Vector2f(0.145f, 0.280f);
    if (city == "LosAngeles")  return sf::Vector2f(0.155f, 0.380f);
    if (city == "Montreal")    return sf::Vector2f(0.275f, 0.290f);
    if (city == "NewYork")     return sf::Vector2f(0.285f, 0.340f);
    
    // --- Europe (Tightly packed, needs precision) ---
    if (city == "Oslo")        return sf::Vector2f(0.495f, 0.210f);
    if (city == "Stockholm")   return sf::Vector2f(0.515f, 0.205f);
    if (city == "Helsinki")    return sf::Vector2f(0.535f, 0.195f);
    if (city == "Copenhagen")  return sf::Vector2f(0.495f, 0.240f);
    if (city == "Hamburg")     return sf::Vector2f(0.490f, 0.260f);
    if (city == "Rotterdam")   return sf::Vector2f(0.482f, 0.275f);
    if (city == "Antwerp")     return sf::Vector2f(0.480f, 0.285f);
    if (city == "London")      return sf::Vector2f(0.465f, 0.280f);
    if (city == "Dublin")      return sf::Vector2f(0.445f, 0.270f);
    if (city == "Lisbon")      return sf::Vector2f(0.435f, 0.360f);
    if (city == "Marseille")   return sf::Vector2f(0.485f, 0.330f);
    if (city == "Genoa")       return sf::Vector2f(0.495f, 0.320f);
    if (city == "Athens")      return sf::Vector2f(0.530f, 0.360f);
    if (city == "Istanbul")    return sf::Vector2f(0.555f, 0.350f);

    // --- Middle East / Africa ---
    if (city == "Alexandria")  return sf::Vector2f(0.545f, 0.400f);
    if (city == "PortSaid")    return sf::Vector2f(0.555f, 0.405f); // Close to Alexandria
    if (city == "Jeddah")      return sf::Vector2f(0.580f, 0.480f);
    if (city == "Dubai")       return sf::Vector2f(0.640f, 0.460f);
    if (city == "AbuDhabi")    return sf::Vector2f(0.635f, 0.465f);
    if (city == "Doha")        return sf::Vector2f(0.630f, 0.455f);
    if (city == "CapeTown")    return sf::Vector2f(0.535f, 0.840f);
    if (city == "Durban")      return sf::Vector2f(0.580f, 0.800f);
    if (city == "PortLouis")   return sf::Vector2f(0.660f, 0.750f);

    // --- Asia ---
    if (city == "Karachi")     return sf::Vector2f(0.665f, 0.460f);
    if (city == "Mumbai")      return sf::Vector2f(0.685f, 0.495f);
    if (city == "Colombo")     return sf::Vector2f(0.710f, 0.560f);
    if (city == "Chittagong")  return sf::Vector2f(0.755f, 0.480f);
    if (city == "Singapore")   return sf::Vector2f(0.795f, 0.600f);
    if (city == "Jakarta")     return sf::Vector2f(0.810f, 0.650f);
    if (city == "Manila")      return sf::Vector2f(0.870f, 0.540f);
    if (city == "HongKong")    return sf::Vector2f(0.840f, 0.480f);
    if (city == "Shanghai")    return sf::Vector2f(0.860f, 0.430f);
    if (city == "Busan")       return sf::Vector2f(0.880f, 0.380f);
    if (city == "Osaka")       return sf::Vector2f(0.895f, 0.390f);
    if (city == "Tokyo")       return sf::Vector2f(0.905f, 0.380f);

    // --- Oceania ---
    if (city == "Melbourne")   return sf::Vector2f(0.890f, 0.860f);
    if (city == "Sydney")      return sf::Vector2f(0.920f, 0.840f);

    return sf::Vector2f(0.5f, 0.5f);
}

float Graphics::getDistance(sf::Vector2f p1, sf::Vector2f p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

float Graphics::distToSegment(sf::Vector2f p, sf::Vector2f v, sf::Vector2f w) {
    float l2 = std::pow(getDistance(v, w), 2);
    if (l2 == 0) return getDistance(p, v);
    float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    t = std::max(0.0f, std::min(1.0f, t));
    sf::Vector2f projection = v + t * (w - v);
    return getDistance(p, projection);
}

Graphics::Graphics(sf::RenderWindow& win, Graph& graph) : window(win) {
    if (!mapTexture.loadFromFile("assets/map.png")) {
        mapTexture.create(1600, 900); 
    }
    mapSprite.setTexture(mapTexture);
    
    // Force precise scaling to window
    float scaleX = (float)window.getSize().x / mapTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mapTexture.getSize().y;
    mapSprite.setScale(scaleX, scaleY);

    // Try loading fonts
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {
                 // Fallback
            }
        }
    }

    // Cache positions
    for (int i = 0; i < graph.ports.getSize(); i++) {
        sf::Vector2f relPos = getRelativeCoordinates(graph.ports[i]->name);
        portScreenPositions.push_back(sf::Vector2f(relPos.x * window.getSize().x, relPos.y * window.getSize().y));
    }
}

int Graphics::handleMouseClick(int mouseX, int mouseY) {
    sf::Vector2f mousePos(mouseX, mouseY);
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        if (getDistance(mousePos, portScreenPositions[i]) < 20.0f) { 
            return i;
        }
    }
    return -1;
}

void Graphics::drawPortQueue(sf::Vector2f pos, int count) {
    if (count <= 0) return;
    sf::RectangleShape box(sf::Vector2f(15, 15));
    box.setFillColor(sf::Color::Yellow);
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(1);
    box.setPosition(pos.x + 8, pos.y - 15); 
    window.draw(box);
}

void Graphics::drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes) {
    window.draw(mapSprite);
    sf::Vector2f mousePos(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    string activeTooltip = "";

    // 1. Draw Routes (Sleeker, Brighter)
    static sf::Clock glowClock; 
    float time = glowClock.getElapsedTime().asSeconds();
    
    // Brighter Alpha: Oscillates between 150 and 255 (never too faded)
    int alpha = static_cast<int>(200 + 55 * sin(time * 5.0f)); 
    
    // Sleeker Gold/Orange Color
    sf::Color glowColor(255, 200, 50, alpha); 
    sf::Color fadeColor(255, 200, 50, 50); // Destination end is semi-transparent

    for (int i = 0; i < graph.ports.getSize(); i++) {
        if (!showAllRoutes && i != selectedPortIndex) continue;

        auto current = graph.ports[i]->routes.head;
        while (current != nullptr) {
            int destIdx = current->data.destinationIndex;
            if (destIdx >= 0 && destIdx < portScreenPositions.getSize()) {
                sf::Vector2f start = portScreenPositions[i];
                sf::Vector2f end = portScreenPositions[destIdx];

                // Use VertexArray for smoother lines if possible, but lines work for now
                sf::Vertex line[] = {
                    sf::Vertex(start, glowColor),
                    sf::Vertex(end, fadeColor)
                };
                window.draw(line, 2, sf::Lines);

                // Route Hover
                if (activeTooltip.empty() && distToSegment(mousePos, start, end) < 5.0f) {
                     std::stringstream ss;
                     ss << "ROUTE: " << graph.ports[i]->name << " -> " << graph.ports[destIdx]->name << "\n"
                        << "Company: " << current->data.company << "\n"
                        << "Cost: $" << current->data.voyageCost << "\n"
                        << "Time: " << setfill('0') << setw(2) << current->data.departureTime.hour << ":" << setw(2) << current->data.departureTime.minute;
                     activeTooltip = ss.str();
                }
            }
            current = current->next;
        }
    }

    // 2. Draw Ports (Modern UI Style)
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        sf::Vector2f pos = portScreenPositions[i];

        // A. The Outer Halo (Transparent Ring)
        sf::CircleShape halo(8.0f);
        halo.setOrigin(8.0f, 8.0f);
        halo.setPosition(pos);
        
        // B. The Inner Core (Solid Dot)
        sf::CircleShape core(3.5f);
        core.setOrigin(3.5f, 3.5f);
        core.setPosition(pos);

        if (i == selectedPortIndex) {
            // Selected: Cyan Core, Pulsing Halo
            halo.setFillColor(sf::Color(0, 255, 255, 100)); // Semi-transparent cyan
            halo.setOutlineColor(sf::Color::Cyan);
            halo.setOutlineThickness(1);
            
            core.setFillColor(sf::Color::White); // Bright white center
        } else {
            // Normal: Red/Orange Aesthetic
            halo.setFillColor(sf::Color(255, 50, 50, 80)); // Faint red halo
            core.setFillColor(sf::Color(255, 80, 80));     // Solid red core
        }

        // Draw the modern dot stack
        window.draw(halo);
        window.draw(core);

        // Hover Check
        if (getDistance(mousePos, portScreenPositions[i]) < 15.0f) {
            std::stringstream ss;
            ss << "PORT: " << graph.ports[i]->name << "\n"
               << "Charges: $" << graph.ports[i]->portCharges;
            activeTooltip = ss.str();
            
            // Highlight on hover
            sf::CircleShape hoverRing(10.0f);
            hoverRing.setOrigin(10.0f, 10.0f);
            hoverRing.setPosition(pos);
            hoverRing.setFillColor(sf::Color::Transparent);
            hoverRing.setOutlineColor(sf::Color::White);
            hoverRing.setOutlineThickness(1);
            window.draw(hoverRing);
        }
    }

    // 3. Tooltip (Modern Box)
    if (!activeTooltip.empty()) {
        sf::Text text(activeTooltip, font, 14); // Slightly smaller, cleaner font size
        sf::FloatRect bounds = text.getGlobalBounds();
        
        float width = bounds.width + 20;
        float height = bounds.height + 16;

        sf::RectangleShape bg(sf::Vector2f(width, height));
        bg.setFillColor(sf::Color(10, 15, 30, 240)); // Deep Navy Blue, almost opaque
        bg.setOutlineColor(sf::Color(100, 200, 255)); // Light Blue Border
        bg.setOutlineThickness(1);
        bg.setPosition(mousePos.x + 15, mousePos.y + 15);
        
        text.setPosition(mousePos.x + 25, mousePos.y + 20);
        text.setFillColor(sf::Color(220, 240, 255)); // Off-white text
        
        window.draw(bg);
        window.draw(text);
    }
}

void Graphics::drawPath(LinkedList<Route>& path) {
    // Future logic
}