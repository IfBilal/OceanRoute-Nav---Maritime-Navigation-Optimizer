#include "Graphics.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// --- Helper Functions ---
sf::Vector2f Graphics::getRelativeCoordinates(string city) {
    if (city == "AbuDhabi")    return sf::Vector2f(0.645f, 0.460f);
    if (city == "Alexandria")  return sf::Vector2f(0.555f, 0.400f);
    if (city == "Doha")        return sf::Vector2f(0.640f, 0.455f);
    if (city == "Dubai")       return sf::Vector2f(0.650f, 0.450f);
    if (city == "Istanbul")    return sf::Vector2f(0.565f, 0.350f);
    if (city == "Jeddah")      return sf::Vector2f(0.590f, 0.480f);
    if (city == "Antwerp")     return sf::Vector2f(0.490f, 0.270f);
    if (city == "Athens")      return sf::Vector2f(0.540f, 0.370f);
    if (city == "Copenhagen")  return sf::Vector2f(0.505f, 0.240f);
    if (city == "Dublin")      return sf::Vector2f(0.450f, 0.260f);
    if (city == "Genoa")       return sf::Vector2f(0.505f, 0.320f);
    if (city == "Hamburg")     return sf::Vector2f(0.500f, 0.260f);
    if (city == "Helsinki")    return sf::Vector2f(0.545f, 0.200f);
    if (city == "Lisbon")      return sf::Vector2f(0.445f, 0.360f);
    if (city == "London")      return sf::Vector2f(0.475f, 0.270f);
    if (city == "Marseille")   return sf::Vector2f(0.495f, 0.330f);
    if (city == "Oslo")        return sf::Vector2f(0.500f, 0.210f);
    if (city == "Rotterdam")   return sf::Vector2f(0.492f, 0.275f);
    if (city == "Stockholm")   return sf::Vector2f(0.530f, 0.210f);
    if (city == "Busan")       return sf::Vector2f(0.890f, 0.380f);
    if (city == "Chittagong")  return sf::Vector2f(0.765f, 0.480f);
    if (city == "Colombo")     return sf::Vector2f(0.720f, 0.560f);
    if (city == "HongKong")    return sf::Vector2f(0.850f, 0.480f);
    if (city == "Jakarta")     return sf::Vector2f(0.820f, 0.660f);
    if (city == "Karachi")     return sf::Vector2f(0.675f, 0.450f);
    if (city == "Manila")      return sf::Vector2f(0.880f, 0.530f);
    if (city == "Mumbai")      return sf::Vector2f(0.695f, 0.490f);
    if (city == "Osaka")       return sf::Vector2f(0.905f, 0.390f);
    if (city == "Shanghai")    return sf::Vector2f(0.870f, 0.430f);
    if (city == "Singapore")   return sf::Vector2f(0.805f, 0.600f);
    if (city == "Tokyo")       return sf::Vector2f(0.915f, 0.380f);
    if (city == "CapeTown")    return sf::Vector2f(0.550f, 0.840f);
    if (city == "Durban")      return sf::Vector2f(0.590f, 0.800f);
    if (city == "PortLouis")   return sf::Vector2f(0.660f, 0.750f);
    if (city == "LosAngeles")  return sf::Vector2f(0.170f, 0.380f);
    if (city == "Montreal")    return sf::Vector2f(0.280f, 0.310f);
    if (city == "NewYork")     return sf::Vector2f(0.290f, 0.350f);
    if (city == "Vancouver")   return sf::Vector2f(0.160f, 0.300f);
    if (city == "Melbourne")   return sf::Vector2f(0.900f, 0.860f);
    if (city == "Sydney")      return sf::Vector2f(0.930f, 0.840f);
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
    
    float scaleX = (float)window.getSize().x / mapTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mapTexture.getSize().y;
    mapSprite.setScale(scaleX, scaleY);

    // Font Loading with Linux Fallbacks
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {
                std::cerr << "Warning: No font found. Text will be invisible." << std::endl;
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

// The function from your screenshot
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

    // 1. Draw Routes
    static sf::Clock glowClock; 
    float time = glowClock.getElapsedTime().asSeconds();
    int alpha = static_cast<int>(177.5 + 77.5 * sin(time * 4.0f)); 
    sf::Color glowColor(255, 165, 0, alpha);

    for (int i = 0; i < graph.ports.getSize(); i++) {
        if (!showAllRoutes && i != selectedPortIndex) continue;

        auto current = graph.ports[i]->routes.head;
        while (current != nullptr) {
            int destIdx = current->data.destinationIndex;
            if (destIdx >= 0 && destIdx < portScreenPositions.getSize()) {
                sf::Vector2f start = portScreenPositions[i];
                sf::Vector2f end = portScreenPositions[destIdx];

                sf::Vertex line[] = {
                    sf::Vertex(start, glowColor),
                    sf::Vertex(end, sf::Color(255, 200, 0, 50))
                };
                window.draw(line, 2, sf::Lines);

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

    // 2. Draw Ports
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        sf::Vector2f pos = portScreenPositions[i];
        sf::CircleShape dot(6.0f); 
        dot.setOrigin(6.0f, 6.0f);
        dot.setPosition(pos);

        if (i == selectedPortIndex) {
            dot.setFillColor(sf::Color::Cyan);
            dot.setOutlineThickness(2);
        } else {
            dot.setFillColor(sf::Color::Red);
        }
        window.draw(dot);

        // Tooltip Logic
        if (getDistance(mousePos, portScreenPositions[i]) < 15.0f) {
            std::stringstream ss;
            ss << "PORT: " << graph.ports[i]->name << "\n"
               << "Charges: $" << graph.ports[i]->portCharges;
            activeTooltip = ss.str();
        }
    }

    // 3. Tooltip
    if (!activeTooltip.empty()) {
        sf::Text text(activeTooltip, font, 16);
        sf::FloatRect bounds = text.getGlobalBounds();
        
        sf::RectangleShape bg(sf::Vector2f(bounds.width + 20, bounds.height + 10));
        bg.setFillColor(sf::Color(0, 0, 0, 230));
        bg.setOutlineColor(sf::Color::Cyan);
        bg.setOutlineThickness(1);
        bg.setPosition(mousePos.x + 15, mousePos.y + 15);
        text.setPosition(mousePos.x + 25, mousePos.y + 20);
        
        window.draw(bg);
        window.draw(text);
    }
}

void Graphics::drawPath(LinkedList<Route>& path) {
    // Future logic
}