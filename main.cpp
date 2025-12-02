#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include "Graph.h"
#include "Graphics.h"

using namespace std;
using namespace sf;

enum AppState { IDLE, INPUTTING_SOURCE, INPUTTING_DEST, INPUTTING_DEPARTURE_DATE, SELECTING_MODE, SELECTING_COMPANY, SELECTING_ALGORITHM, READY_TO_COMPUTE, COMPUTING, SHOWING_PATH };

string stateToString(AppState state) {
    switch(state) {
        case IDLE: return "IDLE";
        case INPUTTING_SOURCE: return "ENTER SOURCE";
        case INPUTTING_DEST: return "ENTER DESTINATION";
        case INPUTTING_DEPARTURE_DATE: return "ENTER DEPARTURE DATE";
        case SELECTING_MODE: return "SELECT MODE";
        case SELECTING_COMPANY: return "SELECT COMPANY";
        case SELECTING_ALGORITHM: return "SELECT ALGORITHM";
        case READY_TO_COMPUTE: return "READY TO COMPUTE";
        case COMPUTING: return "COMPUTING...";
        case SHOWING_PATH: return "PATH FOUND";
        default: return "UNKNOWN";
    }
}

// Helper function to find similar port names
Vector<string> findSimilarPorts(Graph& graph, const string& input) {
    Vector<string> matches;
    if (input.empty()) return matches;
    
    string lowerInput = input;
    for (int i = 0; i < (int)lowerInput.length(); i++) {
        lowerInput[i] = tolower(lowerInput[i]);
    }
    
    for (int i = 0; i < graph.ports.getSize(); i++) {
        string portName = graph.ports[i]->name;
        string lowerPort = portName;
        for (int j = 0; j < (int)lowerPort.length(); j++) {
            lowerPort[j] = tolower(lowerPort[j]);
        }
        
        // Check if port name contains the input or starts with it
        if (lowerPort.find(lowerInput) != string::npos) {
            matches.push_back(portName);
            if (matches.getSize() >= 5) break; // Limit to 5 suggestions
        }
    }
    
    return matches;
}

// Helper function to sanitize input (remove leading/trailing spaces, limit length)
string sanitizeInput(string input) {
    // Remove leading spaces
    int start = 0;
    while (start < (int)input.length() && input[start] == ' ') {
        start++;
    }
    
    // Remove trailing spaces
    int end = input.length();
    while (end > start && input[end - 1] == ' ') {
        end--;
    }
    
    if (start >= end) return "";
    
    string result = input.substr(start, end - start);
    
    // Limit length to 30 characters
    if (result.length() > 30) {
        result = result.substr(0, 30);
    }
    
    return result;
}

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(1600, 900), "OceanRoute Nav - Cyberpunk Edition", Style::Default, settings);
    window.setFramerateLimit(60);

    cout << "Loading Data..." << endl;
    Graph oceanGraph; 
    cout << "Data Loaded." << endl;
    
    Graphics renderer(window, oceanGraph);

    // Audio
    Music backgroundMusic;
    // loop music
    backgroundMusic.openFromFile("assets/music.wav");
    backgroundMusic.setLoop(true);
    backgroundMusic.play();
    // Instructions Text

    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        if(!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {} 
    }

    Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(Color(0, 255, 255)); // Cyan Text
    instructions.setString("Type SOURCE port name, then press ENTER | [T]Toggle Routes [F5]Reset [ESC]Exit");
    
    // Center the text at the top of the screen
    FloatRect textBounds = instructions.getLocalBounds();
    instructions.setOrigin(textBounds.width / 2.0f, 0);
    instructions.setPosition(800, 15);  // 800 is center of 1600px width, 15px from top

    // Text boxes for input with proper spacing
    RectangleShape sourceBox(Vector2f(300, 35));
    sourceBox.setPosition(50, 50);
    sourceBox.setFillColor(Color(20, 25, 40, 220));
    sourceBox.setOutlineColor(Color(0, 255, 255));
    sourceBox.setOutlineThickness(2);
    
    RectangleShape destBox(Vector2f(300, 35));
    destBox.setPosition(50, 110);  // Increased spacing: 50 + 35 + 25 = 110
    destBox.setFillColor(Color(20, 25, 40, 220));
    destBox.setOutlineColor(Color(100, 100, 100));
    destBox.setOutlineThickness(2);
    
    RectangleShape dateBox(Vector2f(300, 35));
    dateBox.setPosition(50, 170);  // Increased spacing: 110 + 35 + 25 = 170
    dateBox.setFillColor(Color(20, 25, 40, 220));
    dateBox.setOutlineColor(Color(100, 100, 100));
    dateBox.setOutlineThickness(2);
    
    Text sourceLabel("SOURCE:", font, 14);
    sourceLabel.setPosition(55, 30);
    sourceLabel.setFillColor(Color(0, 255, 0));
    
    Text destLabel("DESTINATION:", font, 14);
    destLabel.setPosition(55, 90);  // Adjusted for new spacing
    destLabel.setFillColor(Color(255, 0, 0));
    
    Text dateLabel("DEPARTURE DATE:", font, 14);
    dateLabel.setPosition(55, 150);  // Adjusted for new spacing
    dateLabel.setFillColor(Color(100, 150, 255));
    
    Text sourceText("", font, 16);
    sourceText.setPosition(58, 56);
    sourceText.setFillColor(Color::White);
    
    Text destText("", font, 16);
    destText.setPosition(58, 116);  // Adjusted for new box position
    destText.setFillColor(Color::White);
    
    Text dateText("", font, 16);
    dateText.setPosition(58, 176);  // Adjusted for new box position
    dateText.setFillColor(Color::White);
    
    Text cursor("|", font, 16);
    cursor.setFillColor(Color(0, 255, 255));
    
    Clock cursorClock;
    bool showCursor = true;
    
    // Company dropdown UI elements
    Vector<RectangleShape> companyBoxes;
    Vector<Text> companyTexts;
    int hoveredCompanyIndex = -1;
    int selectedCompanyIndex = -1;

    AppState currentState = IDLE;
    int selectedPort = -1;
    int sourcePort = -1;
    int destPort = -1;
    PathResult pathResult;
    DateTime startTime(2024, 12, 1, 8, 0);  // Will be set by user input
    bool showAllRoutes = false; 
    string stateString = "IDLE";
    
    // User input variables
    string sourceInput = "";
    string destInput = "";
    string departureDateInput = "";  // Format: DD/MM/YYYY (date only, no time)
    string selectedCompany = "";  // Empty means auto-find best company
    bool optimizeTime = false;    // false = cost, true = time
    bool userPicksCompany = true; // true = user picks company, false = auto-find best
    
    // Input delay to prevent double key press
    Clock inputDelayClock;
    float INPUT_DELAY = 0.2f; // 200ms delay between state changes
    
    // Get unique companies
    Vector<string> companies;
    for (int i = 0; i < oceanGraph.ports.getSize(); i++) {
        Node<Route>* current = oceanGraph.ports[i]->routes.head;
        while (current != nullptr) {
            bool found = false;
            for (int j = 0; j < companies.getSize(); j++) {
                if (companies[j] == current->data.company) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                companies.push_back(current->data.company);
            }
            current = current->next;
        }
    }
    
    cout << "\n=== AVAILABLE COMPANIES ===" << endl;
    for (int i = 0; i < companies.getSize(); i++) {
        cout << i + 1 << ". " << companies[i] << endl;
    }
    cout << "===========================\n" << endl;
    
    // Initialize company dropdown UI elements
    float dropdownStartY = 200;
    float dropdownX = 50;
    float boxWidth = 350;
    float boxHeight = 40;
    float spacing = 5;
    
    for (int i = 0; i < companies.getSize(); i++) {
        RectangleShape box(Vector2f(boxWidth, boxHeight));
        box.setPosition(dropdownX, dropdownStartY + i * (boxHeight + spacing));
        box.setFillColor(Color(20, 25, 40, 220));
        box.setOutlineColor(Color(100, 100, 100));
        box.setOutlineThickness(2);
        companyBoxes.push_back(box);
        
        Text text(companies[i], font, 16);
        text.setPosition(dropdownX + 15, dropdownStartY + i * (boxHeight + spacing) + 10);
        text.setFillColor(Color::White);
        companyTexts.push_back(text);
    }
    
    Clock dtClock; // Delta Time Clock for smooth animation

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds(); // Get time passed since last frame

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) window.close();
                
                // Toggle showing all routes with 'T' key
                if (event.key.code == Keyboard::T) {
                    showAllRoutes = !showAllRoutes;
                    if (showAllRoutes) {
                        cout << "\n>>> ALL ROUTES VISIBLE (Press T to hide)" << endl;
                    } else {
                        cout << "\n>>> ALL ROUTES HIDDEN (Press T to show)" << endl;
                    }
                }
                
                // Reset (changed from R to F5 to avoid conflict with port names)
                if (event.key.code == Keyboard::F5) {
                    sourcePort = -1;
                    destPort = -1;
                    selectedPort = -1;
                    sourceInput = "";
                    destInput = "";
                    selectedCompany = "";
                    hoveredCompanyIndex = -1;
                    selectedCompanyIndex = -1;
                    currentState = IDLE;
                    stateString = stateToString(currentState);
                    renderer.clearAnimations();
                    pathResult = PathResult();
                    instructions.setString("Type SOURCE port name, then press ENTER | [T]Toggle Routes [F5]Reset [ESC]Exit");
                    textBounds = instructions.getLocalBounds();
                    instructions.setOrigin(textBounds.width / 2.0f, 0);
                    sourceText.setString("");
                    destText.setString("");
                    sourceText.setFillColor(Color::White);
                    destText.setFillColor(Color::White);
                    cout << "\n>>> RESET - Type source port name" << endl;
                }
                
                // Handle ENTER key
                if (event.key.code == Keyboard::Enter) {
                    if (currentState == IDLE && !sourceInput.empty()) {
                        // Sanitize and validate source
                        sourceInput = sanitizeInput(sourceInput);
                        sourcePort = oceanGraph.findPortIndex(sourceInput);
                        
                        if (sourcePort != -1) {
                            currentState = INPUTTING_DEST;
                            stateString = stateToString(currentState);
                            instructions.setString("Type DESTINATION port name, then press ENTER | " + oceanGraph.ports[sourcePort]->name + " -> ???");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            sourceText.setString(oceanGraph.ports[sourcePort]->name);  // Use exact port name
                            sourceText.setFillColor(Color(0, 255, 0));  // Green when validated
                            cout << "[OK] Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << ">>> Type destination port name" << endl;
                        } else {
                            // Port not found - show suggestions
                            cout << "\n[ERROR] ERROR: Port '" << sourceInput << "' not found!" << endl;
                            
                            Vector<string> suggestions = findSimilarPorts(oceanGraph, sourceInput);
                            if (!suggestions.empty()) {
                                cout << "   Did you mean one of these?" << endl;
                                for (int i = 0; i < suggestions.getSize(); i++) {
                                    cout << "   - " << suggestions[i] << endl;
                                }
                            } else {
                                cout << "   No similar ports found. Check spelling." << endl;
                            }
                            
                            sourceInput = "";
                            sourceText.setString("");
                            sourceText.setFillColor(Color(255, 100, 100));  // Light red for error
                            
                            // Update instruction with error
                            instructions.setString("Invalid port! Try again | Examples: Sydney, Mumbai, Rotterdam");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                        }
                    }
                    else if (currentState == INPUTTING_DEST && !destInput.empty()) {
                        // Sanitize and validate destination
                        destInput = sanitizeInput(destInput);
                        destPort = oceanGraph.findPortIndex(destInput);
                        
                        if (destPort != -1 && destPort != sourcePort) {
                            currentState = INPUTTING_DEPARTURE_DATE;
                            stateString = stateToString(currentState);
                            instructions.setString("Enter departure date: DD/MM/YYYY (e.g., 01/12/2024)");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            destText.setString(oceanGraph.ports[destPort]->name);  // Use exact port name
                            destText.setFillColor(Color(255, 0, 0));  // Red when validated
                            cout << "[OK] Destination: " << oceanGraph.ports[destPort]->name << endl;
                            cout << "\n>>> Enter departure date:" << endl;
                            cout << "   Format: DD/MM/YYYY" << endl;
                            cout << "   Example: 01/12/2024" << endl;
                            cout << "   Note: System will automatically select earliest available departure time" << endl;
                        } else if (destPort == sourcePort) {
                            cout << "\n[ERROR] ERROR: Source and destination cannot be the same!" << endl;
                            cout << "   Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "   Please choose a different destination port." << endl;
                            destInput = "";
                            destText.setString("");
                            destText.setFillColor(Color(255, 100, 100));
                            
                            instructions.setString("Error: Same port! Choose different destination");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                        } else {
                            // Port not found - show suggestions
                            cout << "\n[ERROR] ERROR: Port '" << destInput << "' not found!" << endl;
                            
                            Vector<string> suggestions = findSimilarPorts(oceanGraph, destInput);
                            if (!suggestions.empty()) {
                                cout << "   Did you mean one of these?" << endl;
                                for (int i = 0; i < suggestions.getSize(); i++) {
                                    cout << "   - " << suggestions[i] << endl;
                                }
                            } else {
                                cout << "   No similar ports found. Check spelling." << endl;
                            }
                            
                            destInput = "";
                            destText.setString("");
                            destText.setFillColor(Color(255, 100, 100));
                            
                            instructions.setString("Invalid port! Try again | Choose different destination");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                        }
                    }
                    else if (currentState == INPUTTING_DEPARTURE_DATE && !departureDateInput.empty()) {
                        // Parse departure date: DD/MM/YYYY (date only)
                        int day, month, year;
                        char slash1, slash2;
                        
                        stringstream ss(departureDateInput);
                        ss >> day >> slash1 >> month >> slash2 >> year;
                        
                        // Validate: All months have 31 days (simplified calendar system)
                        if (ss.fail() || slash1 != '/' || slash2 != '/' ||
                            day < 1 || day > 31 || month < 1 || month > 12 || year < 2024 || year > 2030) {
                            cout << "\n[ERROR] Invalid date format!" << endl;
                            cout << "   Please use: DD/MM/YYYY" << endl;
                            cout << "   Valid ranges: Day (1-31), Month (1-12), Year (2024-2030)" << endl;
                            cout << "   Example: 01/12/2024" << endl;
                            departureDateInput = "";
                            dateText.setString("");
                            dateText.setFillColor(Color(255, 100, 100));  // Red for error
                            
                            instructions.setString("Invalid date! Use DD/MM/YYYY | Day:1-31, Month:1-12, Year:2024-2030");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                        } else {
                            // Valid date input - use 00:00 (midnight) as default start time
                            // The system will automatically select the earliest available departure on this date
                            // DateTime constructor: DateTime(year, month, day, hour, minute)
                            startTime = DateTime(year, month, day, 0, 0);  // Now correctly using year from user input
                            
                            dateText.setFillColor(Color(0, 255, 0));  // Green for valid
                            
                            currentState = SELECTING_MODE;
                            stateString = stateToString(currentState);
                            instructions.setString("Select mode: [1]Pick Company [2]Multi-Company");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            cout << "[OK] Departure Date: " << day << "/" << month << "/" << year << endl;
                            cout << "   System will select earliest available departure time on this date" << endl;
                            cout << "\n>>> Select pathfinding mode:" << endl;
                            cout << "  1. User-Preferred Company (you pick one company)" << endl;
                            cout << "  2. Multi-Company Path (any combination of companies)" << endl;
                        }
                    }
                    else if (currentState == READY_TO_COMPUTE) {
                        // Compute path
                        currentState = COMPUTING;
                        stateString = stateToString(currentState);
                        
                        // Start exploration visualization
                        renderer.startExploration();
                        
                        if (userPicksCompany) {
                            // Mode 1: User-preferred company
                            cout << "\n========== DIJKSTRA (" << (optimizeTime ? "TIME" : "COST") << " OPTIMIZATION) ==========" << endl;
                            cout << "Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "Destination: " << oceanGraph.ports[destPort]->name << endl;
                            cout << "Company: " << selectedCompany << endl;
                            
                            pathResult = oceanGraph.findPathDijkstra(sourcePort, destPort, startTime, optimizeTime, selectedCompany);
                            
                            // Set exploration data for visualization
                            renderer.setExplorationData(pathResult);
                        }
                        else {
                            // Mode 2: Multi-company path (no company filter)
                            cout << "\n========== DIJKSTRA (" << (optimizeTime ? "TIME" : "COST") << " - MULTI-COMPANY PATH) ==========" << endl;
                            cout << "Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "Destination: " << oceanGraph.ports[destPort]->name << endl;
                            cout << "Allowing any combination of companies..." << endl;
                            
                            pathResult = oceanGraph.findPathDijkstra(sourcePort, destPort, startTime, optimizeTime, "");
                            
                            // Set exploration data for visualization
                            renderer.setExplorationData(pathResult);
                        }
                        
                        if (pathResult.pathFound) {
                            currentState = SHOWING_PATH;
                            stateString = stateToString(currentState);
                            
                            // Show BOTH cost and time in instructions
                            string costStr = "$" + to_string((int)pathResult.totalCost);
                            string timeStr = to_string((int)pathResult.totalTime) + "h";
                            instructions.setString("PATH FOUND! Cost: " + costStr + " | Time: " + timeStr + " | [F5]Reset");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Start ship animation along the path
                            renderer.startShipAnimation(pathResult.routes, oceanGraph);
                            
                            cout << "\n[OK] =============== PATH FOUND =============== [OK]" << endl;
                            cout << "Total Cost: $" << pathResult.totalCost << endl;
                            cout << "Total Time: " << pathResult.totalTime << " hours (" << (int)(pathResult.totalTime / 24) << " days)" << endl;
                            cout << "Nodes Explored: " << pathResult.nodesExplored << " / " << oceanGraph.ports.getSize() << endl;
                            
                            // Count hops
                            int hops = 0;
                            Node<Route>* counter = pathResult.routes.head;
                            while (counter) {
                                hops++;
                                counter = counter->next;
                            }
                            cout << "Number of Hops: " << hops << endl;
                            
                            cout << "\nDetailed Route:" << endl;
                            // Print route
                            Node<Route>* routeNode = pathResult.routes.head;
                            int hopNum = 1;
                            while (routeNode != nullptr) {
                                cout << "  " << hopNum << ". " << routeNode->data.sourcePortName 
                                     << " → " << oceanGraph.ports[routeNode->data.destinationIndex]->name
                                     << " [" << routeNode->data.company << "]"
                                     << " ($" << routeNode->data.voyageCost << ")" << endl;
                                hopNum++;
                                routeNode = routeNode->next;
                            }
                            cout << "============================================\n" << endl;
                        } else {
                            // No path found - provide detailed error
                            currentState = IDLE;
                            stateString = stateToString(currentState);
                            sourceInput = "";
                            destInput = "";
                            sourceText.setString("");
                            destText.setString("");
                            sourceText.setFillColor(Color::White);
                            destText.setFillColor(Color::White);
                            
                            instructions.setString("NO PATH FOUND! Check connectivity or try different mode | [R]Reset");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            cout << "\n[ERROR] ============== PATH NOT FOUND ============== [ERROR]" << endl;
                            cout << "ERROR: No route exists between:" << endl;
                            cout << "  Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "  Destination: " << oceanGraph.ports[destPort]->name << endl;
                            
                            if (userPicksCompany) {
                                cout << "\nPossible reasons:" << endl;
                                cout << "  • Company '" << selectedCompany << "' doesn't operate routes to this destination" << endl;
                                cout << "  • No connecting routes available for this company" << endl;
                                cout << "  • The ports are in disconnected network segments" << endl;
                                cout << "\nSuggestions:" << endl;
                                cout << "  1. Try Mode 2 (Multi-Company Path)" << endl;
                                cout << "  2. Try Mode 3 (Auto-find Best Company)" << endl;
                                cout << "  3. Select a different company" << endl;
                                cout << "  4. Choose different source/destination ports" << endl;
                            } else if (selectedCompany == "AUTO") {
                                cout << "\nPossible reasons:" << endl;
                                cout << "  • No single company operates a complete route" << endl;
                                cout << "  • The ports are in disconnected network segments" << endl;
                                cout << "  • Routes may exist but not through any single company" << endl;
                                cout << "\nSuggestions:" << endl;
                                cout << "  1. Try Mode 2 (Multi-Company Path) - allows mixing companies" << endl;
                                cout << "  2. Check if both ports exist in the network" << endl;
                                cout << "  3. Try different source/destination ports" << endl;
                            } else {
                                cout << "\nPossible reasons:" << endl;
                                cout << "  • The ports are in completely disconnected network segments" << endl;
                                cout << "  • No routes exist in the data between these regions" << endl;
                                cout << "\nSuggestions:" << endl;
                                cout << "  1. Verify both ports exist in PortCharges.txt" << endl;
                                cout << "  2. Check Routes.txt for connectivity" << endl;
                                cout << "  3. Try different source/destination ports" << endl;
                            }
                            cout << "============================================\n" << endl;
                        }
                    }
                }
                
                // Handle Backspace
                if (event.key.code == Keyboard::BackSpace) {
                    if (currentState == IDLE && sourceInput.length() > 0) {
                        sourceInput.pop_back();
                        sourceText.setString(sourceInput);
                    }
                    else if (currentState == INPUTTING_DEST && destInput.length() > 0) {
                        destInput.pop_back();
                        destText.setString(destInput);
                    }
                }
                
                // Handle mode selection (User pick vs Multi-company vs Auto-find)
                if (currentState == SELECTING_MODE && inputDelayClock.getElapsedTime().asSeconds() > INPUT_DELAY) {
                    if (event.key.code == Keyboard::Num1) {
                        userPicksCompany = true;
                        currentState = SELECTING_COMPANY;
                        stateString = stateToString(currentState);
                        instructions.setString("Click on a company from the dropdown list");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << ">>> Selected: USER-PREFERRED COMPANY mode" << endl;
                        cout << ">>> Click on a company to select" << endl;
                        inputDelayClock.restart(); // Reset delay
                    }
                    else if (event.key.code == Keyboard::Num2) {
                        // Multi-company mode - no company filter
                        userPicksCompany = false;
                        selectedCompany = "";  // Empty = allow all companies
                        currentState = SELECTING_ALGORITHM;
                        stateString = stateToString(currentState);
                        instructions.setString("Select optimization: [1]Cost [2]Time");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << ">>> Selected: MULTI-COMPANY PATH mode (any companies allowed)" << endl;
                        cout << ">>> Select optimization: [1]Cost [2]Time" << endl;
                        inputDelayClock.restart(); // Reset delay
                    }
                }
                
                // Handle algorithm selection
                if (currentState == SELECTING_ALGORITHM && inputDelayClock.getElapsedTime().asSeconds() > INPUT_DELAY) {
                    if (event.key.code == Keyboard::Num1) {
                        optimizeTime = false;
                        currentState = READY_TO_COMPUTE;
                        stateString = stateToString(currentState);
                        instructions.setString("COST optimization selected | Press ENTER to compute path");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << "[OK] Selected: COST optimization" << endl;
                        cout << "\n>>> Press ENTER to start pathfinding..." << endl;
                        inputDelayClock.restart(); // Reset delay
                    }
                    else if (event.key.code == Keyboard::Num2) {
                        optimizeTime = true;
                        currentState = READY_TO_COMPUTE;
                        stateString = stateToString(currentState);
                        instructions.setString("TIME optimization selected | Press ENTER to compute path");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << "[OK] Selected: TIME optimization" << endl;
                        cout << "\n>>> Press ENTER to start pathfinding..." << endl;
                        inputDelayClock.restart(); // Reset delay
                    }
                }
            }
            
            // Handle text input
            if (event.type == Event::TextEntered) {
                // Skip Enter key (13) - handled separately in KeyPressed
                if (event.text.unicode == 13) {
                    // Skip - ENTER is handled in KeyPressed event
                }
                // Handle Backspace (8)
                else if (event.text.unicode == 8) {
                    if (currentState == IDLE && sourceInput.length() > 0) {
                        sourceInput = sourceInput.substr(0, sourceInput.length() - 1);
                        sourceText.setString(sourceInput);
                    }
                    else if (currentState == INPUTTING_DEST && destInput.length() > 0) {
                        destInput = destInput.substr(0, destInput.length() - 1);
                        destText.setString(destInput);
                    }
                    else if (currentState == INPUTTING_DEPARTURE_DATE && departureDateInput.length() > 0) {
                        departureDateInput = departureDateInput.substr(0, departureDateInput.length() - 1);
                        dateText.setString(departureDateInput);
                    }
                }
                else if (currentState == IDLE) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Only allow letters, numbers, and spaces, limit to 30 characters
                    if ((isalnum(inputChar) || inputChar == ' ') && sourceInput.length() < 30) {
                        sourceInput += inputChar;
                        sourceText.setString(sourceInput);
                        sourceText.setFillColor(Color::White);  // Keep white while typing
                    }
                    // Ignore invalid characters silently
                }
                else if (currentState == INPUTTING_DEST) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Only allow letters, numbers, and spaces, limit to 30 characters
                    if ((isalnum(inputChar) || inputChar == ' ') && destInput.length() < 30) {
                        destInput += inputChar;
                        destText.setString(destInput);
                        destText.setFillColor(Color::White);  // Keep white while typing
                    }
                    // Ignore invalid characters silently
                }
                else if (currentState == INPUTTING_DEPARTURE_DATE) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Allow digits and slashes for date input (DD/MM/YYYY)
                    if ((isdigit(inputChar) || inputChar == '/') && departureDateInput.length() < 10) {
                        departureDateInput += inputChar;
                        dateText.setString(departureDateInput);
                        dateText.setFillColor(Color::White);
                    }
                    // Ignore invalid characters silently
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    // Check if clicking on company dropdown (only in SELECTING_COMPANY state)
                    if (currentState == SELECTING_COMPANY) {
                        bool companyClicked = false;
                        for (int i = 0; i < companyBoxes.getSize(); i++) {
                            if (companyBoxes[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                                selectedCompany = companies[i];
                                selectedCompanyIndex = i;
                                currentState = SELECTING_ALGORITHM;
                                stateString = stateToString(currentState);
                                instructions.setString("Select optimization: [1]Cost [2]Time");
                                textBounds = instructions.getLocalBounds();
                                instructions.setOrigin(textBounds.width / 2.0f, 0);
                                cout << ">>> Selected: " << selectedCompany << endl;
                                cout << ">>> Select optimization: [1]Cost [2]Time" << endl;
                                companyClicked = true;
                                break;
                            }
                        }
                        if (companyClicked) {
                            // Skip port click handling when company is clicked
                            continue;
                        }
                    }
                    
                    // Handle port clicks
                    int clickedIndex = renderer.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    
                    if (clickedIndex != -1) {
                        selectedPort = clickedIndex;
                        cout << "Browsing Port: " << oceanGraph.ports[clickedIndex]->name << endl;
                        
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
                        selectedPort = -1;
                        renderer.clearAnimations(); // Clear lines if clicking empty space
                    }
                }
            }
            
            // Handle mouse movement for hover effects
            if (event.type == Event::MouseMoved) {
                if (currentState == SELECTING_COMPANY) {
                    hoveredCompanyIndex = -1;
                    for (int i = 0; i < companyBoxes.getSize(); i++) {
                        if (companyBoxes[i].getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                            hoveredCompanyIndex = i;
                            break;
                        }
                    }
                }
            }
        }
        
        // Update company dropdown visual states
        if (currentState == SELECTING_COMPANY) {
            for (int i = 0; i < companyBoxes.getSize(); i++) {
                if (i == hoveredCompanyIndex) {
                    companyBoxes[i].setOutlineColor(Color(0, 255, 255));
                    companyBoxes[i].setOutlineThickness(3);
                    companyBoxes[i].setFillColor(Color(30, 40, 60, 240));
                    companyTexts[i].setFillColor(Color(0, 255, 255));
                } else {
                    companyBoxes[i].setOutlineColor(Color(100, 100, 100));
                    companyBoxes[i].setOutlineThickness(2);
                    companyBoxes[i].setFillColor(Color(20, 25, 40, 220));
                    companyTexts[i].setFillColor(Color::White);
                }
            }
        } else {
            // Reset hover when not in company selection
            hoveredCompanyIndex = -1;
        }

        // Update Animations
        renderer.update(dt);
        
        // Update cursor blinking
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
        
        // Update text box highlights based on current state
        if (currentState == IDLE) {
            sourceBox.setOutlineColor(Color(0, 255, 255));
            sourceBox.setOutlineThickness(2);
            destBox.setOutlineColor(Color(100, 100, 100));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(100, 100, 100));
            dateBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_DEST) {
            sourceBox.setOutlineColor(Color(100, 100, 100));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(0, 255, 255));
            destBox.setOutlineThickness(2);
            dateBox.setOutlineColor(Color(100, 100, 100));
            dateBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_DEPARTURE_DATE) {
            sourceBox.setOutlineColor(Color(100, 100, 100));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(100, 100, 100));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(0, 255, 255));
            dateBox.setOutlineThickness(2);
        } else {
            sourceBox.setOutlineColor(Color(100, 100, 100));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(100, 100, 100));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(100, 100, 100));
            dateBox.setOutlineThickness(1);
        }

        window.clear(Color(5, 10, 20)); // Very dark blue background
        renderer.drawWorld(oceanGraph, selectedPort, showAllRoutes, sourcePort, destPort);
        
        // Draw HUD panel
        renderer.drawHUD(sourcePort, destPort, pathResult, oceanGraph, stateString, "Dijkstra");
        
        // Draw computed path if found
        if (pathResult.pathFound) {
            renderer.drawPath(pathResult.routes, oceanGraph);
        }
        
        // Draw text input boxes
        window.draw(sourceLabel);
        window.draw(destLabel);
        window.draw(dateLabel);
        window.draw(sourceBox);
        window.draw(destBox);
        window.draw(dateBox);
        window.draw(sourceText);
        window.draw(destText);
        window.draw(dateText);
        
        // Draw blinking cursor
        if (showCursor) {
            if (currentState == IDLE) {
                FloatRect sourceBounds = sourceText.getLocalBounds();
                cursor.setPosition(58 + sourceBounds.width + 2, 56);
                window.draw(cursor);
            } else if (currentState == INPUTTING_DEST) {
                FloatRect destBounds = destText.getLocalBounds();
                cursor.setPosition(58 + destBounds.width + 2, 116);
                window.draw(cursor);
            } else if (currentState == INPUTTING_DEPARTURE_DATE) {
                FloatRect dateBounds = dateText.getLocalBounds();
                cursor.setPosition(58 + dateBounds.width + 2, 176);
                window.draw(cursor);
            }
        }
        
        // Draw company dropdown if in SELECTING_COMPANY state
        if (currentState == SELECTING_COMPANY) {
            // Draw semi-transparent background overlay
            RectangleShape overlay(Vector2f(1600, 900));
            overlay.setFillColor(Color(0, 0, 0, 150));
            window.draw(overlay);
            
            // Draw title
            Text dropdownTitle("SELECT SHIPPING COMPANY", font, 24);
            dropdownTitle.setFillColor(Color(0, 255, 255));
            dropdownTitle.setStyle(Text::Bold);
            FloatRect titleBounds = dropdownTitle.getLocalBounds();
            dropdownTitle.setPosition(50, 150);
            window.draw(dropdownTitle);
            
            // Draw company boxes
            for (int i = 0; i < companyBoxes.getSize(); i++) {
                window.draw(companyBoxes[i]);
                window.draw(companyTexts[i]);
            }
        }
        
        window.draw(instructions);
        window.display();
    }

    return 0;
}