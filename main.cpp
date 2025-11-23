#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Graph.h"
#include "Graphics.h"

using namespace std;

enum AppState { IDLE, SELECTING_SOURCE, SELECTING_DEST, SHOWING_PATH };

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1600, 900), "OceanRoute Nav - Cyberpunk Edition", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    cout << "Loading Data..." << endl;
    Graph oceanGraph; 
    cout << "Data Loaded." << endl;
    
    Graphics renderer(window, oceanGraph);

    // Audio
    sf::Music backgroundMusic;
    if (backgroundMusic.openFromFile("assets/music.wav")) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(70);
        backgroundMusic.play();
    }

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        if(!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {} 
    }

    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(18);
    instructions.setFillColor(sf::Color(0, 255, 255)); // Cyan Text
    instructions.setString("Controls: [Left Click] Select Port | [T] Toggle All Routes | [ESC] Exit");
    instructions.setPosition(20, 900 - 40);

    AppState currentState = IDLE;
    int selectedPort = -1;      
    bool showAllRoutes = false; 
    
    sf::Clock dtClock; // Delta Time Clock for smooth animation

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds(); // Get time passed since last frame

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) window.close();
                if (event.key.code == sf::Keyboard::T) showAllRoutes = !showAllRoutes;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int clickedIndex = renderer.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    
                    if (clickedIndex != -1) {
                        selectedPort = clickedIndex;
                        cout << "Clicked Port: " << oceanGraph.ports[clickedIndex]->name << endl;
                        
                        // --- ANIMATION TRIGGER ---
                        // Clear old lines
                        renderer.clearAnimations(); 
                        
                        // Find all routes STARTING from this port and animate them
                        Node<Route>* current = oceanGraph.ports[clickedIndex]->routes.head;
                        while(current != nullptr) {
                            // Start animation for this route
                            renderer.startRouteAnimation(
                                clickedIndex, 
                                current->data.destinationIndex, 
                                current->data
                            );
                            current = current->next;
                        }

                    } else {
                        if (currentState == IDLE) {
                            selectedPort = -1;
                            renderer.clearAnimations(); // Clear lines if clicking empty space
                        }
                    }
                }
            }
        }

        // Update Animations
        renderer.update(dt);

        window.clear(sf::Color(5, 10, 20)); // Very dark blue background
        renderer.drawWorld(oceanGraph, selectedPort, showAllRoutes);
        window.draw(instructions);
        window.display();
    }

    return 0;
}