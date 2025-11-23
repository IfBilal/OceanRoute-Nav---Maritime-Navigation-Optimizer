#include "Graphics.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// --- Constants ---
const sf::Color CYBER_CYAN(0, 255, 255);
const sf::Color CYBER_BLUE(0, 150, 255);
const sf::Color CYBER_DARK(10, 15, 30, 240);

// UPDATED: Golden/Amber color for the "Show All" network
// RGB(255, 180, 50) is Gold. Alpha 40 makes it subtle but visible.
const sf::Color GLOBAL_NET_COLOR(255, 180, 50, 40); 

const float ANIMATION_SPEED = 1.2f;

// --- Helper Functions ---
sf::Vector2f Graphics::getRelativeCoordinates(string city) {
    // --- Americas ---
    if (city == "Vancouver")   return sf::Vector2f(0.145f, 0.280f);
    if (city == "LosAngeles")  return sf::Vector2f(0.155f, 0.380f);
    if (city == "Montreal")    return sf::Vector2f(0.275f, 0.290f);
    if (city == "NewYork")     return sf::Vector2f(0.285f, 0.340f);
    
    // --- Europe ---
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
    if (city == "PortSaid")    return sf::Vector2f(0.555f, 0.405f);
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
    if (city == "Singapore")   return sf::Vector2f(0.790f, 0.590f);
    if (city == "Jakarta")     return sf::Vector2f(0.810f, 0.640f);
    if (city == "Manila")      return sf::Vector2f(0.865f, 0.530f);
    if (city == "HongKong")    return sf::Vector2f(0.840f, 0.480f);
    if (city == "Shanghai")    return sf::Vector2f(0.860f, 0.430f);
    if (city == "Busan")       return sf::Vector2f(0.880f, 0.380f);
    if (city == "Osaka")       return sf::Vector2f(0.895f, 0.390f);
    if (city == "Tokyo")       return sf::Vector2f(0.900f, 0.370f);

    // --- Oceania ---
    if (city == "Melbourne")   return sf::Vector2f(0.875f, 0.810f);
    if (city == "Sydney")      return sf::Vector2f(0.905f, 0.790f);

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

    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {}
        }
    }

    for (int i = 0; i < graph.ports.getSize(); i++) {
        sf::Vector2f relPos = getRelativeCoordinates(graph.ports[i]->name);
        portScreenPositions.push_back(sf::Vector2f(relPos.x * window.getSize().x, relPos.y * window.getSize().y));
    }
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
}

void Graphics::update(float deltaTime) {
    for (int i = 0; i < activeAnimations.getSize(); i++) {
        if (activeAnimations[i].progress < 1.0f) {
            activeAnimations[i].progress += activeAnimations[i].speed * deltaTime;
            if (activeAnimations[i].progress > 1.0f) activeAnimations[i].progress = 1.0f;
        }
    }
}

// --- Drawing ---
void Graphics::drawWorld(Graph& graph, int selectedPortIndex, bool showAllRoutes) {
    window.draw(mapSprite);
    sf::Vector2f mousePos(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    string activeTooltip = "";

    // 1. Draw Global Routes (If 'T' is pressed) - Golden Web
    if (showAllRoutes) {
        for (int i = 0; i < graph.ports.getSize(); i++) {
            auto current = graph.ports[i]->routes.head;
            while (current != nullptr) {
                int destIdx = current->data.destinationIndex;
                
                // Use the new GOLDEN color here
                sf::Vertex line[] = {
                    sf::Vertex(portScreenPositions[i], GLOBAL_NET_COLOR), 
                    sf::Vertex(portScreenPositions[destIdx], GLOBAL_NET_COLOR)
                };
                window.draw(line, 2, sf::Lines);
                current = current->next;
            }
        }
    }

    // 2. Draw Active Animations (Glowing Cyan)
    for (int i = 0; i < activeAnimations.getSize(); i++) {
        RouteAnimation& anim = activeAnimations[i];
        sf::Vector2f start = portScreenPositions[anim.sourceIdx];
        sf::Vector2f end = portScreenPositions[anim.destIdx];
        sf::Vector2f currentTip = start + (end - start) * anim.progress;

        sf::Vertex line[] = {
            sf::Vertex(start, anim.color),
            sf::Vertex(currentTip, sf::Color(0, 150, 255, 150)) 
        };
        
        window.draw(line, 2, sf::Lines);
        line[0].position.x += 1; line[1].position.x += 1; 
        window.draw(line, 2, sf::Lines);
        line[0].position.y += 1; line[1].position.y += 1; 
        window.draw(line, 2, sf::Lines);
        
        sf::CircleShape head(3.0f);
        head.setOrigin(3.0f, 3.0f);
        head.setPosition(currentTip);
        head.setFillColor(sf::Color::White);
        window.draw(head);

        if (activeTooltip.empty() && distToSegment(mousePos, start, end) < 8.0f) {
             std::stringstream ss;
             ss << "ROUTE: " << graph.ports[anim.sourceIdx]->name << " -> " << graph.ports[anim.destIdx]->name << "\n"
                << "Company: " << anim.company << "\n"
                << "Cost: $" << anim.cost;
             activeTooltip = ss.str();
        }
    }

    // 3. Draw Ports
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        sf::Vector2f pos = portScreenPositions[i];
        
        static sf::Clock pulseClock;
        float time = pulseClock.getElapsedTime().asSeconds();
        float pulseScale = 1.0f + 0.2f * sin(time * 3.0f + i); 
        
        sf::CircleShape halo(7.0f * pulseScale);
        halo.setOrigin(halo.getRadius(), halo.getRadius());
        halo.setPosition(pos);
        
        sf::CircleShape core(3.0f);
        core.setOrigin(3.0f, 3.0f);
        core.setPosition(pos);

        if (i == selectedPortIndex) {
            halo.setFillColor(sf::Color(0, 255, 255, 120)); 
            halo.setOutlineColor(CYBER_CYAN);
            halo.setOutlineThickness(1);
            core.setFillColor(sf::Color::White);
        } else {
            halo.setFillColor(sf::Color(0, 100, 200, 50)); 
            core.setFillColor(CYBER_CYAN);
        }

        window.draw(halo);
        window.draw(core);

        if (getDistance(mousePos, portScreenPositions[i]) < 15.0f) {
            std::stringstream ss;
            ss << "PORT: " << graph.ports[i]->name << "\n"
               << "Charges: $" << graph.ports[i]->portCharges;
            activeTooltip = ss.str();
            
            sf::CircleShape hoverRing(12.0f);
            hoverRing.setOrigin(12.0f, 12.0f);
            hoverRing.setPosition(pos);
            hoverRing.setFillColor(sf::Color::Transparent);
            hoverRing.setOutlineColor(sf::Color::White);
            hoverRing.setOutlineThickness(1);
            window.draw(hoverRing);
        }
    }

    // 4. Draw Tooltip
    if (!activeTooltip.empty()) {
        sf::Text text(activeTooltip, font, 14);
        sf::FloatRect bounds = text.getGlobalBounds();
        sf::RectangleShape bg(sf::Vector2f(bounds.width + 20, bounds.height + 16));
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
    sf::Vector2f mousePos(mouseX, mouseY);
    for (int i = 0; i < portScreenPositions.getSize(); i++) {
        if (getDistance(mousePos, portScreenPositions[i]) < 20.0f) return i;
    }
    return -1;
}

void Graphics::drawPortQueue(sf::Vector2f pos, int count) {
    if (count <= 0) return;
    sf::RectangleShape box(sf::Vector2f(12, 12));
    box.setFillColor(sf::Color::Yellow);
    box.setPosition(pos.x + 10, pos.y - 10); 
    window.draw(box);
}

void Graphics::drawPath(LinkedList<Route>& path) {
    // Future Feature
}