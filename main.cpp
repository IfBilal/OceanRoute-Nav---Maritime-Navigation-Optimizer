#include <SFML/Graphics.hpp>
#include <iostream>
#include "Graph.h"
#include "Graphics.h"

using namespace std;

enum AppState {
    IDLE,
    SELECTING_SOURCE,
    SELECTING_DEST,
    SHOWING_PATH
};

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1600, 900), "OceanRoute Nav", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    cout << "Loading Data..." << endl;
    Graph oceanGraph; 
    cout << "Data Loaded." << endl;
    
    Graphics renderer(window, oceanGraph);

    // FIX: Use ::vector here too
    ::vector<sf::Vector2f> portScreenPositions; 

    AppState currentState = IDLE;
    int selectedPort = -1;      
    int sourcePort = -1;        
    int destPort = -1;          
    bool showAllRoutes = false; 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) 
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    if (currentState == SHOWING_PATH) currentState = IDLE;
                    else window.close();
                }

                if (event.key.code == sf::Keyboard::T) {
                    showAllRoutes = !showAllRoutes;
                }

                if (event.key.code == sf::Keyboard::B) {
                    cout << "Booking Mode: Select Source Port..." << endl;
                    currentState = SELECTING_SOURCE;
                    selectedPort = -1;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int clickedIndex = renderer.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    
                    if (clickedIndex != -1) {
                        selectedPort = clickedIndex;
                        cout << "Clicked Port: " << oceanGraph.ports[clickedIndex]->name << endl;

                        if (currentState == SELECTING_SOURCE) {
                            sourcePort = clickedIndex;
                            cout << "Source Set. Select Destination." << endl;
                            currentState = SELECTING_DEST;
                        } 
                        else if (currentState == SELECTING_DEST) {
                            destPort = clickedIndex;
                            cout << "Destination Set. Calculating Path..." << endl;
                            currentState = SHOWING_PATH;
                        }
                    } else {
                        if (currentState == IDLE) selectedPort = -1;
                    }
                }
            }
        }

        window.clear(sf::Color(10, 10, 30));
        renderer.drawWorld(oceanGraph, selectedPort, showAllRoutes);
        window.display();
    }

    return 0;
}