#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include "Graph.h"
#include "Graphics.h"

using namespace std;
using namespace sf;

enum AppState { MAIN_MENU, SELECTING_PATHFINDING_ALGO, SELECTING_ALGO_OPTIONS, INPUTTING_AVOID_PORTS, INPUTTING_MAX_VOYAGE_TIME, IDLE, INPUTTING_SOURCE, INPUTTING_DEST, INPUTTING_DEPARTURE_DATE, SELECTING_MODE, SELECTING_COMPANY, SELECTING_ALGORITHM, READY_TO_COMPUTE, COMPUTING, SHOWING_PATH };

string stateToString(AppState state) {
    switch(state) {
        case MAIN_MENU: return "MAIN MENU";
        case SELECTING_PATHFINDING_ALGO: return "SELECT PATHFINDING ALGORITHM";
        case SELECTING_ALGO_OPTIONS: return "SELECT ALGORITHM OPTIONS";
        case INPUTTING_AVOID_PORTS: return "ENTER AVOID PORTS";
        case INPUTTING_MAX_VOYAGE_TIME: return "ENTER MAX VOYAGE TIME";
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
    backgroundMusic.openFromFile("assets/music.wav");
    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(200);
    backgroundMusic.play();
    
    // UI Sound Effects
    SoundBuffer hoverBuffer, clickBuffer;
    if (!hoverBuffer.loadFromFile("assets/buttonhoverandclicksound.wav")) {
        cout << "Warning: Could not load hover sound" << endl;
    }
    if (!clickBuffer.loadFromFile("assets/buttonhoverandclicksound.wav")) {
        cout << "Warning: Could not load click sound" << endl;
    }
    
    Sound hoverSound(hoverBuffer);
    hoverSound.setVolume(80);
    Sound clickSound(clickBuffer);
    clickSound.setVolume(150);
    
    int lastHoveredAlgo = -1;
    int lastHoveredCompany = -1;
    // Instructions Text

    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        if(!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {} 
    }

    Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(Color(200, 200, 200));
    instructions.setString("Main Menu | Select Algorithm");
    
    // Instruction background panel - simplified
    RectangleShape instructionBg(Vector2f(0, 0));
    instructionBg.setFillColor(Color(10, 10, 15, 200));
    instructionBg.setOutlineColor(Color(60, 60, 60));
    instructionBg.setOutlineThickness(1);
    
    // Center the text at the top of the screen
    FloatRect textBounds = instructions.getLocalBounds();
    instructions.setOrigin(textBounds.width / 2.0f, 0);
    instructions.setPosition(800, 10);

    // Text boxes for input - simplified
    RectangleShape sourceBox(Vector2f(300, 35));
    sourceBox.setPosition(50, 50);
    sourceBox.setFillColor(Color(20, 20, 25, 200));
    sourceBox.setOutlineColor(Color(80, 80, 80));
    sourceBox.setOutlineThickness(1);
    
    RectangleShape destBox(Vector2f(300, 35));
    destBox.setPosition(50, 110);
    destBox.setFillColor(Color(20, 20, 25, 200));
    destBox.setOutlineColor(Color(80, 80, 80));
    destBox.setOutlineThickness(1);
    
    RectangleShape dateBox(Vector2f(300, 35));
    dateBox.setPosition(50, 170);
    dateBox.setFillColor(Color(20, 20, 25, 200));
    dateBox.setOutlineColor(Color(80, 80, 80));
    dateBox.setOutlineThickness(1);
    
    Text sourceLabel("SOURCE:", font, 12);
    sourceLabel.setPosition(55, 32);
    sourceLabel.setFillColor(Color(150, 150, 150));
    
    Text destLabel("DESTINATION:", font, 12);
    destLabel.setPosition(55, 92);
    destLabel.setFillColor(Color(150, 150, 150));
    
    Text dateLabel("DEPARTURE DATE:", font, 12);
    dateLabel.setPosition(55, 152);
    dateLabel.setFillColor(Color(150, 150, 150));
    
    Text sourceText("", font, 16);
    sourceText.setPosition(58, 56);
    sourceText.setFillColor(Color::White);
    
    Text destText("", font, 16);
    destText.setPosition(58, 116);  // Adjusted for new box position
    destText.setFillColor(Color::White);
    
    Text dateText("", font, 16);
    dateText.setPosition(58, 176);  // Adjusted for new box position
    dateText.setFillColor(Color::White);
    
    // Additional input boxes for algorithm options
    RectangleShape avoidPortsBox(Vector2f(500, 35));
    avoidPortsBox.setPosition(550, 400);
    avoidPortsBox.setFillColor(Color(20, 20, 25, 200));
    avoidPortsBox.setOutlineColor(Color(80, 80, 80));
    avoidPortsBox.setOutlineThickness(1);
    
    RectangleShape maxTimeBox(Vector2f(500, 35));
    maxTimeBox.setPosition(550, 490);
    maxTimeBox.setFillColor(Color(20, 20, 25, 200));
    maxTimeBox.setOutlineColor(Color(80, 80, 80));
    maxTimeBox.setOutlineThickness(1);
    
    Text avoidPortsLabel("Ports to avoid:", font, 12);
    avoidPortsLabel.setPosition(555, 382);
    avoidPortsLabel.setFillColor(Color(150, 150, 150));
    
    Text maxTimeLabel("Max hours:", font, 12);
    maxTimeLabel.setPosition(555, 472);
    maxTimeLabel.setFillColor(Color(150, 150, 150));
    
    Text avoidPortsText("", font, 14);
    avoidPortsText.setPosition(558, 406);
    avoidPortsText.setFillColor(Color::White);
    
    Text maxTimeText("", font, 14);
    maxTimeText.setPosition(558, 496);
    maxTimeText.setFillColor(Color::White);
    
    Text cursor("|", font, 18);
    cursor.setFillColor(Color(0, 255, 255));
    cursor.setStyle(Text::Bold);
    
    // Warning/Error display panel - simplified
    Text warningText("", font, 18);
    warningText.setFillColor(Color(255, 100, 100));
    warningText.setPosition(800, 300);
    RectangleShape warningBg(Vector2f(0, 0));
    warningBg.setFillColor(Color(40, 20, 20, 220));
    warningBg.setOutlineColor(Color(200, 80, 80));
    warningBg.setOutlineThickness(2);
    bool showWarning = false;
    Clock warningClock;
    
    // Placeholder/hint text for input fields - SIMPLE
    Text sourcePlaceholder("Sydney, Mumbai...", font, 12);
    sourcePlaceholder.setPosition(60, 58);
    sourcePlaceholder.setFillColor(Color(80, 80, 80));
    
    Text destPlaceholder("Tokyo, London...", font, 12);
    destPlaceholder.setPosition(60, 118);
    destPlaceholder.setFillColor(Color(80, 80, 80));
    
    Text datePlaceholder("DD/MM/YYYY", font, 12);
    datePlaceholder.setPosition(60, 178);
    datePlaceholder.setFillColor(Color(80, 80, 80));
    
    Text avoidPortsPlaceholder("Mumbai, Dubai, Singapore...", font, 12);
    avoidPortsPlaceholder.setPosition(560, 408);
    avoidPortsPlaceholder.setFillColor(Color(80, 80, 80));
    
    Text maxTimePlaceholder("120", font, 12);
    maxTimePlaceholder.setPosition(560, 498);
    maxTimePlaceholder.setFillColor(Color(80, 80, 80));
    
    Clock cursorClock;
    bool showCursor = true;
    
    // Company dropdown UI elements
    Vector<RectangleShape> companyBoxes;
    Vector<Text> companyTexts;
    int hoveredCompanyIndex = -1;
    int selectedCompanyIndex = -1;
    
    // Pathfinding algorithm selection
    Vector<string> algorithms;
    algorithms.push_back("Dijkstra");
    algorithms.push_back("A* (A-Star)");
    algorithms.push_back("DFS (All Paths)");
    int hoveredAlgoIndex = -1;
    int selectedAlgoIndex = -1;  // 0 = Dijkstra, 1 = A*, 2 = DFS
    string selectedAlgorithmName = "Dijkstra";

    AppState currentState = MAIN_MENU;
    int selectedPort = -1;
    int sourcePort = -1;
    int destPort = -1;
    PathResult pathResult;
    AllPathsResult allPathsResult;  // For DFS results
    int selectedPathIndex = 0;  // Which path to display from allPathsResult
    DateTime startTime(2024, 12, 1, 8, 0);  // Will be set by user input
    bool showAllRoutes = false; 
    string stateString = "MAIN MENU";
    
    // User input variables
    string sourceInput = "";
    string destInput = "";
    string departureDateInput = "";  // Format: DD/MM/YYYY (date only, no time)
    string selectedCompany = "";  // Empty means auto-find best company
    bool optimizeTime = false;    // false = cost, true = time
    bool userPicksCompany = true; // true = user picks company, false = auto-find best
    
    // Algorithm options
    Vector<string> avoidPortsInput;  // List of ports to avoid
    string avoidPortsStr = "";  // Current input string for avoid ports
    string maxVoyageTimeStr = "";  // Input string for max voyage time
    double maxVoyageTime = -1;  // -1 = no limit
    bool useAvoidPorts = false;
    bool useMaxVoyageTime = false;
    
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
    
    // Initialize company dropdown UI elements - simplified
    float dropdownStartY = 200;
    float dropdownX = 50;
    float boxWidth = 300;
    float boxHeight = 35;
    float spacing = 3;
    
    for (int i = 0; i < companies.getSize(); i++) {
        RectangleShape box(Vector2f(boxWidth, boxHeight));
        box.setPosition(dropdownX, dropdownStartY + i * (boxHeight + spacing));
        box.setFillColor(Color(20, 20, 25, 200));
        box.setOutlineColor(Color(80, 80, 80));
        box.setOutlineThickness(1);
        companyBoxes.push_back(box);
        
        Text text(companies[i], font, 14);
        text.setPosition(dropdownX + 10, dropdownStartY + i * (boxHeight + spacing) + 8);
        text.setFillColor(Color::White);
        companyTexts.push_back(text);
    }
    
    // Initialize algorithm dropdown UI elements
    Vector<RectangleShape> algoBoxes;
    Vector<Text> algoTexts;
    
    float algoDropdownStartY = 350;
    float algoDropdownX = 550;
    float algoBoxWidth = 400;
    float algoBoxHeight = 50;
    float algoSpacing = 10;
    
    for (int i = 0; i < algorithms.getSize(); i++) {
        RectangleShape box(Vector2f(algoBoxWidth, algoBoxHeight));
        box.setPosition(algoDropdownX, algoDropdownStartY + i * (algoBoxHeight + algoSpacing));
        box.setFillColor(Color(20, 25, 40, 220));
        box.setOutlineColor(Color(100, 100, 100));
        box.setOutlineThickness(2);
        algoBoxes.push_back(box);
        
        Text text(algorithms[i], font, 20);
        text.setPosition(algoDropdownX + 20, algoDropdownStartY + i * (algoBoxHeight + algoSpacing) + 13);
        text.setFillColor(Color::White);
        algoTexts.push_back(text);
    }
    
    Clock dtClock; // Delta Time Clock for smooth animation

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds(); // Get time passed since last frame

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) window.close();
                
                // Toggle showing all routes with 'T' key - ONLY when NOT typing
                if (event.key.code == Keyboard::T && 
                    currentState != INPUTTING_SOURCE && 
                    currentState != INPUTTING_DEST && 
                    currentState != INPUTTING_DEPARTURE_DATE &&
                    currentState != INPUTTING_AVOID_PORTS &&
                    currentState != INPUTTING_MAX_VOYAGE_TIME) {
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
                    departureDateInput = "";
                    selectedCompany = "";
                    hoveredCompanyIndex = -1;
                    selectedCompanyIndex = -1;
                    hoveredAlgoIndex = -1;
                    selectedAlgoIndex = -1;
                    avoidPortsInput.clear();
                    avoidPortsStr = "";
                    maxVoyageTimeStr = "";
                    maxVoyageTime = -1;
                    useAvoidPorts = false;
                    useMaxVoyageTime = false;
                    currentState = MAIN_MENU;
                    stateString = stateToString(currentState);
                    renderer.clearAnimations();
                    pathResult = PathResult();
                    instructions.setString("Main Menu | Select Algorithm");
                    textBounds = instructions.getLocalBounds();
                    instructions.setOrigin(textBounds.width / 2.0f, 0);
                    sourceText.setString("");
                    destText.setString("");
                    dateText.setString("");
                    sourceText.setFillColor(Color::White);
                    destText.setFillColor(Color::White);
                    dateText.setFillColor(Color::White);
                    cout << "\n>>> RESET - Back to main menu" << endl;
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
                            sourceText.setFillColor(Color(255, 0, 0));  // Red for error
                            
                            // Update instruction with error - RED and visible
                            instructions.setString(">>> ERROR: Invalid port! Try again | Examples: Sydney, Mumbai, Rotterdam");
                            instructions.setFillColor(Color(255, 0, 0));  // RED warning
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Show warning panel
                            showWarning = true;
                            warningText.setString("ERROR: Port '" + sanitizeInput(sourceInput) + "' not found!");
                            FloatRect wBounds = warningText.getLocalBounds();
                            warningText.setOrigin(wBounds.width / 2.0f, 0);
                            warningBg.setSize(Vector2f(wBounds.width + 60, wBounds.height + 40));
                            warningBg.setPosition(800 - (wBounds.width + 60) / 2, 275);
                            warningClock.restart();
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
                            instructions.setFillColor(Color(0, 255, 255));  // Reset to cyan
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
                            destText.setFillColor(Color(255, 0, 0));
                            
                            instructions.setString(">>> ERROR: Same port! Choose different destination");
                            instructions.setFillColor(Color(255, 0, 0));  // RED warning
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Show warning panel
                            showWarning = true;
                            warningText.setString("ERROR: Source and Destination cannot be the same!");
                            FloatRect wBounds = warningText.getLocalBounds();
                            warningText.setOrigin(wBounds.width / 2.0f, 0);
                            warningBg.setSize(Vector2f(wBounds.width + 60, wBounds.height + 40));
                            warningBg.setPosition(800 - (wBounds.width + 60) / 2, 275);
                            warningClock.restart();
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
                            destText.setFillColor(Color(255, 0, 0));
                            
                            instructions.setString(">>> ERROR: Invalid port! Try again | Choose different destination");
                            instructions.setFillColor(Color(255, 0, 0));  // RED warning
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Show warning panel
                            showWarning = true;
                            warningText.setString("ERROR: Port '" + sanitizeInput(destInput) + "' not found!");
                            FloatRect wBounds = warningText.getLocalBounds();
                            warningText.setOrigin(wBounds.width / 2.0f, 0);
                            warningBg.setSize(Vector2f(wBounds.width + 60, wBounds.height + 40));
                            warningBg.setPosition(800 - (wBounds.width + 60) / 2, 275);
                            warningClock.restart();
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
                            dateText.setFillColor(Color(255, 0, 0));  // Red for error
                            
                            instructions.setString(">>> ERROR: Invalid date! Use DD/MM/YYYY | Day:1-31, Month:1-12, Year:2024-2030");
                            instructions.setFillColor(Color(255, 0, 0));  // RED warning
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Show warning panel
                            showWarning = true;
                            warningText.setString("ERROR: Invalid date format! Use DD/MM/YYYY");
                            FloatRect wBounds = warningText.getLocalBounds();
                            warningText.setOrigin(wBounds.width / 2.0f, 0);
                            warningBg.setSize(Vector2f(wBounds.width + 60, wBounds.height + 40));
                            warningBg.setPosition(800 - (wBounds.width + 60) / 2, 275);
                            warningClock.restart();
                        } else {
                            // Valid date input - use 00:00 (midnight) as default start time
                            // The system will automatically select the earliest available departure on this date
                            // DateTime constructor: DateTime(year, month, day, hour, minute)
                            startTime = DateTime(year, month, day, 0, 0);  // Now correctly using year from user input
                            
                            dateText.setFillColor(Color(0, 255, 0));  // Green for valid
                            
                            // Show options menu after date input
                            currentState = SELECTING_ALGO_OPTIONS;
                            stateString = stateToString(currentState);
                            instructions.setString("Select options: [1]Avoid Ports [2]Max Voyage Time [3]Continue");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            cout << "[OK] Departure Date: " << day << "/" << month << "/" << year << endl;
                            cout << "   System will select earliest available departure time on this date" << endl;
                            cout << "\n>>> Select pathfinding options:" << endl;
                            cout << "  [1] Add ports to avoid" << endl;
                            cout << "  [2] Set maximum voyage time limit" << endl;
                            cout << "  [3] Continue without options" << endl;
                        }
                    }
                    else if (currentState == INPUTTING_AVOID_PORTS) {
                        // Parse avoid ports (comma-separated) - allow empty to skip
                        if (!avoidPortsStr.empty()) {
                            avoidPortsInput.clear();
                            string temp = avoidPortsStr;
                            while (!temp.empty()) {
                                int commaPos = temp.find(',');
                                string portName;
                                if (commaPos != string::npos) {
                                    portName = temp.substr(0, commaPos);
                                    temp = temp.substr(commaPos + 1);
                                } else {
                                    portName = temp;
                                    temp = "";
                                }
                                
                                // Trim spaces
                                portName = sanitizeInput(portName);
                                
                                // Validate port exists
                                if (!portName.empty()) {
                                    int portIdx = oceanGraph.findPortIndex(portName);
                                    if (portIdx != -1) {
                                        avoidPortsInput.push_back(oceanGraph.ports[portIdx]->name);
                                        cout << "[OK] Will avoid port: " << oceanGraph.ports[portIdx]->name << endl;
                                    } else {
                                        cout << "[WARNING] Port '" << portName << "' not found, skipping" << endl;
                                    }
                                }
                            }
                            useAvoidPorts = true;
                        } else {
                            cout << "[OK] Skipping avoid ports option" << endl;
                        }
                        
                        // Clear input field and return to options
                        avoidPortsStr = "";
                        avoidPortsText.setString("");
                        currentState = SELECTING_ALGO_OPTIONS;
                        stateString = stateToString(currentState);
                        instructions.setString("Options: [1]Avoid Ports [2]Max Time [3]Continue");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << "\n>>> Select next option or continue:" << endl;
                        cout << "  [1] Configure ports to avoid" << endl;
                        cout << "  [2] Set maximum voyage time" << endl;
                        cout << "  [3] Continue to route planning" << endl;
                    }
                    else if (currentState == INPUTTING_MAX_VOYAGE_TIME) {
                        // Parse max voyage time - allow empty to skip
                        if (!maxVoyageTimeStr.empty()) {
                            try {
                                maxVoyageTime = stod(maxVoyageTimeStr);
                                if (maxVoyageTime <= 0) {
                                    cout << "[ERROR] Maximum voyage time must be positive!" << endl;
                                    maxVoyageTimeStr = "";
                                    maxVoyageTime = -1;
                                } else {
                                    cout << "[OK] Maximum voyage time set to: " << maxVoyageTime << " hours (" 
                                         << (int)(maxVoyageTime / 24) << " days)" << endl;
                                    useMaxVoyageTime = true;
                                }
                            } catch (...) {
                                cout << "[ERROR] Invalid number format!" << endl;
                                maxVoyageTimeStr = "";
                                maxVoyageTime = -1;
                            }
                        } else {
                            cout << "[OK] Skipping max voyage time option" << endl;
                        }
                        
                        // Clear input field and return to options
                        maxVoyageTimeStr = "";
                        maxTimeText.setString("");
                        currentState = SELECTING_ALGO_OPTIONS;
                        stateString = stateToString(currentState);
                        instructions.setString("Options: [1]Avoid Ports [2]Max Time [3]Continue");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << "\n>>> Select next option or continue:" << endl;
                        cout << "  [1] Configure ports to avoid" << endl;
                        cout << "  [2] Set maximum voyage time" << endl;
                        cout << "  [3] Continue to route planning" << endl;
                    }
                    else if (currentState == READY_TO_COMPUTE) {
                        // Compute path
                        currentState = COMPUTING;
                        stateString = stateToString(currentState);
                        
                        // DFS uses different result structure
                        if (selectedAlgoIndex == 2) {
                            // DFS - Find All Paths
                            string sourceName = oceanGraph.ports[sourcePort]->name;
                            string destName = oceanGraph.ports[destPort]->name;
                            
                            cout << "\n========== DFS - FINDING ALL PATHS ==========" << endl;
                            cout << "Source: " << sourceName << endl;
                            cout << "Destination: " << destName << endl;
                            if (useAvoidPorts && !avoidPortsInput.empty()) {
                                cout << "Avoiding ports: ";
                                for (int i = 0; i < avoidPortsInput.getSize(); i++) {
                                    cout << avoidPortsInput[i];
                                    if (i < avoidPortsInput.getSize() - 1) cout << ", ";
                                }
                                cout << endl;
                            }
                            if (useMaxVoyageTime && maxVoyageTime > 0) {
                                cout << "Maximum voyage time: " << maxVoyageTime << " hours" << endl;
                            }
                            cout << "Searching for all possible paths..." << endl;
                            
                            allPathsResult = oceanGraph.findAllPaths(sourceName, destName, startTime, avoidPortsInput, maxVoyageTime);
                            
                            if (allPathsResult.totalPathsFound > 0) {
                                selectedPathIndex = 0;  // Start with first path
                                pathResult = allPathsResult.allPaths[0];
                                
                                // Set exploration data for visualization
                                renderer.setExplorationData(pathResult);
                                
                                cout << "\n[OK] Found " << allPathsResult.totalPathsFound << " path(s)!" << endl;
                                cout << "Displaying path 1/" << allPathsResult.totalPathsFound << endl;
                            } else {
                                pathResult = PathResult();  // Empty result
                            }
                        }
                        else if (userPicksCompany) {
                            // Mode 1: User-preferred company (Dijkstra/A*)
                            cout << "\n========== " << selectedAlgorithmName << " (" << (optimizeTime ? "TIME" : "COST") << " OPTIMIZATION) ==========" << endl;
                            cout << "Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "Destination: " << oceanGraph.ports[destPort]->name << endl;
                            cout << "Company: " << selectedCompany << endl;
                            if (useAvoidPorts && !avoidPortsInput.empty()) {
                                cout << "Avoiding ports: ";
                                for (int i = 0; i < avoidPortsInput.getSize(); i++) {
                                    cout << avoidPortsInput[i];
                                    if (i < avoidPortsInput.getSize() - 1) cout << ", ";
                                }
                                cout << endl;
                            }
                            if (useMaxVoyageTime && maxVoyageTime > 0) {
                                cout << "Maximum voyage time: " << maxVoyageTime << " hours" << endl;
                            }
                            
                            // Call the appropriate algorithm
                            if (selectedAlgoIndex == 0) {
                                // Dijkstra
                                pathResult = oceanGraph.findPathDijkstra(sourcePort, destPort, startTime, optimizeTime, selectedCompany, avoidPortsInput, maxVoyageTime);
                            } else {
                                // A*
                                pathResult = oceanGraph.findPathAStar(sourcePort, destPort, startTime, optimizeTime, selectedCompany, avoidPortsInput, maxVoyageTime);
                            }
                            
                            // Set exploration data for visualization
                            renderer.setExplorationData(pathResult);
                        }
                        else {
                            // Mode 2: Multi-company path (Dijkstra/A*)
                            cout << "\n========== " << selectedAlgorithmName << " (" << (optimizeTime ? "TIME" : "COST") << " - MULTI-COMPANY PATH) ==========" << endl;
                            cout << "Source: " << oceanGraph.ports[sourcePort]->name << endl;
                            cout << "Destination: " << oceanGraph.ports[destPort]->name << endl;
                            cout << "Allowing any combination of companies..." << endl;
                            if (useAvoidPorts && !avoidPortsInput.empty()) {
                                cout << "Avoiding ports: ";
                                for (int i = 0; i < avoidPortsInput.getSize(); i++) {
                                    cout << avoidPortsInput[i];
                                    if (i < avoidPortsInput.getSize() - 1) cout << ", ";
                                }
                                cout << endl;
                            }
                            if (useMaxVoyageTime && maxVoyageTime > 0) {
                                cout << "Maximum voyage time: " << maxVoyageTime << " hours" << endl;
                            }
                            
                            // Call the appropriate algorithm
                            if (selectedAlgoIndex == 0) {
                                // Dijkstra
                                pathResult = oceanGraph.findPathDijkstra(sourcePort, destPort, startTime, optimizeTime, "", avoidPortsInput, maxVoyageTime);
                            } else {
                                // A*
                                pathResult = oceanGraph.findPathAStar(sourcePort, destPort, startTime, optimizeTime, "", avoidPortsInput, maxVoyageTime);
                            }
                            
                            // Set exploration data for visualization
                            renderer.setExplorationData(pathResult);
                        }
                        
                        if (pathResult.pathFound) {
                            currentState = SHOWING_PATH;
                            stateString = stateToString(currentState);
                            
                            // Show BOTH cost and time in instructions
                            string costStr = "$" + to_string((int)pathResult.totalCost);
                            string timeStr = to_string((int)pathResult.totalTime) + "h";
                            
                            if (selectedAlgoIndex == 2) {
                                // DFS - show which path we're viewing
                                instructions.setString("PATH " + to_string(selectedPathIndex + 1) + "/" + to_string(allPathsResult.totalPathsFound) + 
                                                     " | Cost: " + costStr + " | Time: " + timeStr + " | [LEFT/RIGHT]Switch [F5]Reset");
                            } else {
                                instructions.setString("PATH FOUND! Cost: " + costStr + " | Time: " + timeStr + " | [F5]Reset");
                            }
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            // Start ship animation along the path
                            renderer.startShipAnimation(pathResult.routes, oceanGraph);
                            
                            cout << "\n[OK] =============== PATH FOUND =============== [OK]" << endl;
                            if (selectedAlgoIndex == 2) {
                                cout << "Displaying path " << selectedPathIndex + 1 << " of " << allPathsResult.totalPathsFound << endl;
                            }
                            cout << "Total Cost: $" << pathResult.totalCost << endl;
                            cout << "Total Time: " << pathResult.totalTime << " hours (" << (int)(pathResult.totalTime / 24) << " days)" << endl;
                            if (selectedAlgoIndex == 2) {
                                cout << "Total Nodes Explored (all paths): " << allPathsResult.nodesExplored << " / " << oceanGraph.ports.getSize() << endl;
                            } else {
                                cout << "Nodes Explored: " << pathResult.nodesExplored << " / " << oceanGraph.ports.getSize() << endl;
                            }
                            
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
                            
                            if (selectedAlgoIndex == 2 && allPathsResult.totalPathsFound > 1) {
                                cout << "\n[INFO] Use LEFT/RIGHT arrow keys to browse through " << allPathsResult.totalPathsFound << " paths" << endl;
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
                            
                            instructions.setString(">>> ERROR: NO PATH FOUND! Check connectivity or try different mode | [F5]Reset");
                            instructions.setFillColor(Color(255, 0, 0));  // RED warning
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
                    else if (currentState == INPUTTING_DEPARTURE_DATE && departureDateInput.length() > 0) {
                        departureDateInput.pop_back();
                        dateText.setString(departureDateInput);
                    }
                    else if (currentState == INPUTTING_AVOID_PORTS && avoidPortsStr.length() > 0) {
                        avoidPortsStr.pop_back();
                        avoidPortsText.setString(avoidPortsStr);
                    }
                    else if (currentState == INPUTTING_MAX_VOYAGE_TIME && maxVoyageTimeStr.length() > 0) {
                        maxVoyageTimeStr.pop_back();
                        maxTimeText.setString(maxVoyageTimeStr);
                    }
                }
                
                // Handle mode selection (User pick vs Multi-company vs Auto-find)
                // ONLY process 1/2 in SELECTING_MODE state to avoid conflicts
                if (currentState == SELECTING_MODE && inputDelayClock.getElapsedTime().asSeconds() > INPUT_DELAY) {
                    if (event.key.code == Keyboard::Num1 || event.key.code == Keyboard::Numpad1) {
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
                // ONLY process 1/2 in SELECTING_ALGORITHM state to avoid conflicts
                if (currentState == SELECTING_ALGORITHM && inputDelayClock.getElapsedTime().asSeconds() > INPUT_DELAY) {
                    if (event.key.code == Keyboard::Num1 || event.key.code == Keyboard::Numpad1) {
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
                    else if (event.key.code == Keyboard::Num2 || event.key.code == Keyboard::Numpad2) {
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
                
                // Handle algorithm options selection
                if (currentState == SELECTING_ALGO_OPTIONS && inputDelayClock.getElapsedTime().asSeconds() > INPUT_DELAY) {
                    if (event.key.code == Keyboard::Num1 || event.key.code == Keyboard::Numpad1) {
                        // Avoid specific ports
                        useAvoidPorts = true;
                        currentState = INPUTTING_AVOID_PORTS;
                        stateString = stateToString(currentState);
                        instructions.setString("Enter port names to avoid (comma-separated), then press ENTER | e.g., Mumbai,Dubai");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << ">>> Enter ports to avoid (comma-separated):" << endl;
                        cout << "   Example: Mumbai,Dubai,Singapore" << endl;
                        cout << "   Press ENTER when done" << endl;
                        inputDelayClock.restart();
                    }
                    else if (event.key.code == Keyboard::Num2 || event.key.code == Keyboard::Numpad2) {
                        // Max voyage time
                        useMaxVoyageTime = true;
                        currentState = INPUTTING_MAX_VOYAGE_TIME;
                        stateString = stateToString(currentState);
                        instructions.setString("Enter maximum voyage time in hours, then press ENTER | e.g., 120");
                        textBounds = instructions.getLocalBounds();
                        instructions.setOrigin(textBounds.width / 2.0f, 0);
                        cout << ">>> Enter maximum voyage time in hours:" << endl;
                        cout << "   Example: 120 (5 days)" << endl;
                        cout << "   Press ENTER when done" << endl;
                        inputDelayClock.restart();
                    }
                    else if (event.key.code == Keyboard::Num3 || event.key.code == Keyboard::Numpad3) {
                        // Continue without options or with configured options
                        if (selectedAlgoIndex == 2) {
                            // DFS goes straight to ready to compute
                            currentState = READY_TO_COMPUTE;
                            stateString = stateToString(currentState);
                            instructions.setString("DFS will find ALL paths | Press ENTER to compute");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            cout << ">>> DFS will explore all possible paths" << endl;
                            if (useAvoidPorts && !avoidPortsInput.empty()) {
                                cout << "   Avoiding ports: ";
                                for (int i = 0; i < avoidPortsInput.getSize(); i++) {
                                    cout << avoidPortsInput[i];
                                    if (i < avoidPortsInput.getSize() - 1) cout << ", ";
                                }
                                cout << endl;
                            }
                            if (useMaxVoyageTime && maxVoyageTime > 0) {
                                cout << "   Maximum voyage time: " << maxVoyageTime << " hours" << endl;
                            }
                            cout << ">>> Press ENTER to start..." << endl;
                        } else {
                            // Dijkstra/A* go to mode selection
                            currentState = SELECTING_MODE;
                            stateString = stateToString(currentState);
                            instructions.setString("Select mode: [1]Pick Company [2]Multi-Company");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            cout << ">>> Continuing to mode selection" << endl;
                            if (useAvoidPorts && !avoidPortsInput.empty()) {
                                cout << "   Avoiding ports: ";
                                for (int i = 0; i < avoidPortsInput.getSize(); i++) {
                                    cout << avoidPortsInput[i];
                                    if (i < avoidPortsInput.getSize() - 1) cout << ", ";
                                }
                                cout << endl;
                            }
                            if (useMaxVoyageTime && maxVoyageTime > 0) {
                                cout << "   Maximum voyage time: " << maxVoyageTime << " hours" << endl;
                            }
                            cout << "\n>>> Select pathfinding mode:" << endl;
                            cout << "  1. User-Preferred Company (you pick one company)" << endl;
                            cout << "  2. Multi-Company Path (any combination of companies)" << endl;
                        }
                        inputDelayClock.restart();
                    }
                }
                
                // Handle arrow keys for DFS path switching
                if (currentState == SHOWING_PATH && selectedAlgoIndex == 2 && allPathsResult.totalPathsFound > 1) {
                    if (event.key.code == Keyboard::Left) {
                        // Previous path
                        if (selectedPathIndex > 0) {
                            selectedPathIndex--;
                            pathResult = allPathsResult.allPaths[selectedPathIndex];
                            
                            // Update visualization
                            renderer.clearAnimations();
                            renderer.setExplorationData(pathResult);
                            renderer.startShipAnimation(pathResult.routes, oceanGraph);
                            
                            // Update instructions
                            string costStr = "$" + to_string((int)pathResult.totalCost);
                            string timeStr = to_string((int)pathResult.totalTime) + "h";
                            instructions.setString("PATH " + to_string(selectedPathIndex + 1) + "/" + to_string(allPathsResult.totalPathsFound) + 
                                                 " | Cost: " + costStr + " | Time: " + timeStr + " | [LEFT/RIGHT]Switch [F5]Reset");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            cout << "\n>>> Showing path " << selectedPathIndex + 1 << " of " << allPathsResult.totalPathsFound << endl;
                            cout << "Cost: $" << pathResult.totalCost << " | Time: " << pathResult.totalTime << "h" << endl;
                        }
                    }
                    else if (event.key.code == Keyboard::Right) {
                        // Next path
                        if (selectedPathIndex < allPathsResult.totalPathsFound - 1) {
                            selectedPathIndex++;
                            pathResult = allPathsResult.allPaths[selectedPathIndex];
                            
                            // Update visualization
                            renderer.clearAnimations();
                            renderer.setExplorationData(pathResult);
                            renderer.startShipAnimation(pathResult.routes, oceanGraph);
                            
                            // Update instructions
                            string costStr = "$" + to_string((int)pathResult.totalCost);
                            string timeStr = to_string((int)pathResult.totalTime) + "h";
                            instructions.setString("PATH " + to_string(selectedPathIndex + 1) + "/" + to_string(allPathsResult.totalPathsFound) + 
                                                 " | Cost: " + costStr + " | Time: " + timeStr + " | [LEFT/RIGHT]Switch [F5]Reset");
                            textBounds = instructions.getLocalBounds();
                            instructions.setOrigin(textBounds.width / 2.0f, 0);
                            
                            cout << "\n>>> Showing path " << selectedPathIndex + 1 << " of " << allPathsResult.totalPathsFound << endl;
                            cout << "Cost: $" << pathResult.totalCost << " | Time: " << pathResult.totalTime << "h" << endl;
                        }
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
                    else if (currentState == INPUTTING_AVOID_PORTS && avoidPortsStr.length() > 0) {
                        avoidPortsStr = avoidPortsStr.substr(0, avoidPortsStr.length() - 1);
                        avoidPortsText.setString(avoidPortsStr);
                    }
                    else if (currentState == INPUTTING_MAX_VOYAGE_TIME && maxVoyageTimeStr.length() > 0) {
                        maxVoyageTimeStr = maxVoyageTimeStr.substr(0, maxVoyageTimeStr.length() - 1);
                        maxTimeText.setString(maxVoyageTimeStr);
                    }
                }
                else if (currentState == IDLE) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Only allow letters and spaces (NO numbers to avoid conflicts with 1/2 keys)
                    if ((isalpha(inputChar) || inputChar == ' ') && sourceInput.length() < 30) {
                        sourceInput += inputChar;
                        sourceText.setString(sourceInput);
                        sourceText.setFillColor(Color::White);  // Keep white while typing
                    }
                    // Ignore invalid characters silently
                }
                else if (currentState == INPUTTING_DEST) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Only allow letters and spaces (NO numbers to avoid conflicts with 1/2 keys)
                    if ((isalpha(inputChar) || inputChar == ' ') && destInput.length() < 30) {
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
                else if (currentState == INPUTTING_AVOID_PORTS) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Allow letters, spaces, and commas
                    if ((isalpha(inputChar) || inputChar == ' ' || inputChar == ',') && avoidPortsStr.length() < 100) {
                        avoidPortsStr += inputChar;
                        avoidPortsText.setString(avoidPortsStr);
                        avoidPortsText.setFillColor(Color::White);
                    }
                }
                else if (currentState == INPUTTING_MAX_VOYAGE_TIME) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    // Allow digits and decimal point
                    if ((isdigit(inputChar) || inputChar == '.') && maxVoyageTimeStr.length() < 10) {
                        maxVoyageTimeStr += inputChar;
                        maxTimeText.setString(maxVoyageTimeStr);
                        maxTimeText.setFillColor(Color::White);
                    }
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    // Handle main menu clicks
                    if (currentState == MAIN_MENU) {
                        // Check if clicking on algorithm buttons (Dijkstra, A*, DFS)
                        float buttonY = 350;
                        float buttonWidth = 400;
                        float buttonHeight = 60;
                        float spacing = 20;
                        float buttonX = 800 - buttonWidth / 2;
                        
                        for (int i = 0; i < 3; i++) {
                            float y = buttonY + i * (buttonHeight + spacing);
                            FloatRect bounds(buttonX, y, buttonWidth, buttonHeight);
                            
                            if (bounds.contains(event.mouseButton.x, event.mouseButton.y)) {
                                clickSound.play();
                                selectedAlgoIndex = i;
                                if (i == 0) selectedAlgorithmName = "Dijkstra";
                                else if (i == 1) selectedAlgorithmName = "A* (A-Star)";
                                else selectedAlgorithmName = "DFS (All Paths)";
                                
                                currentState = IDLE;
                                stateString = stateToString(currentState);
                                instructions.setString("Type SOURCE port name, then press ENTER | [T]Toggle Routes [F5]Reset [ESC]Exit");
                                textBounds = instructions.getLocalBounds();
                                instructions.setOrigin(textBounds.width / 2.0f, 0);
                                cout << "\n>>> Selected Algorithm: " << selectedAlgorithmName << endl;
                                cout << ">>> Type source port name" << endl;
                                break;
                            }
                        }
                    }
                    
                    // Check if clicking on algorithm dropdown (only in SELECTING_PATHFINDING_ALGO state)
                    if (currentState == SELECTING_PATHFINDING_ALGO) {
                        bool algoClicked = false;
                        
                        // Use actual menu bounds from drawAlgorithmMenu
                        float startY = 380;
                        float boxWidth = 450;
                        float boxHeight = 60;
                        float spacing = 15;
                        float startX = 800 - boxWidth / 2;
                        
                        for (int i = 0; i < algorithms.getSize(); i++) {
                            float y = startY + i * (boxHeight + spacing);
                            FloatRect bounds(startX, y, boxWidth, boxHeight);
                            
                            if (bounds.contains(event.mouseButton.x, event.mouseButton.y)) {
                                clickSound.play();
                                selectedAlgoIndex = i;
                                selectedAlgorithmName = algorithms[i];
                                currentState = IDLE;
                                stateString = stateToString(currentState);
                                instructions.setString("Type SOURCE port name, then press ENTER | [T]Toggle Routes [F5]Reset [ESC]Exit");
                                instructions.setFillColor(Color(0, 255, 255));  // Reset to cyan
                                textBounds = instructions.getLocalBounds();
                                instructions.setOrigin(textBounds.width / 2.0f, 0);
                                cout << "\n>>> Selected Algorithm: " << selectedAlgorithmName << endl;
                                cout << ">>> Type source port name" << endl;
                                algoClicked = true;
                                break;
                            }
                        }
                        if (algoClicked) {
                            continue;
                        }
                    }
                    
                    // Check if clicking on company dropdown (only in SELECTING_COMPANY state)
                    if (currentState == SELECTING_COMPANY) {
                        bool companyClicked = false;
                        for (int i = 0; i < companyBoxes.getSize(); i++) {
                            if (companyBoxes[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                                clickSound.play();
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
                if (currentState == MAIN_MENU) {
                    // Check if hovering over START button
                    FloatRect startButtonBounds(575, 450, 450, 70);
                    static bool wasHovering = false;
                    bool isHovering = startButtonBounds.contains(event.mouseMove.x, event.mouseMove.y);
                    
                    if (isHovering && !wasHovering) {
                        hoverSound.play();
                    }
                    wasHovering = isHovering;
                }
                
                if (currentState == SELECTING_PATHFINDING_ALGO) {
                    int prevHovered = hoveredAlgoIndex;
                    hoveredAlgoIndex = -1;
                    
                    // Use actual menu bounds from drawAlgorithmMenu
                    float startY = 380;
                    float boxWidth = 450;
                    float boxHeight = 60;
                    float spacing = 15;
                    float startX = 800 - boxWidth / 2;
                    
                    for (int i = 0; i < algorithms.getSize(); i++) {
                        float y = startY + i * (boxHeight + spacing);
                        FloatRect bounds(startX, y, boxWidth, boxHeight);
                        
                        if (bounds.contains(event.mouseMove.x, event.mouseMove.y)) {
                            hoveredAlgoIndex = i;
                            break;
                        }
                    }
                    
                    // Play hover sound when hovering new item
                    if (hoveredAlgoIndex != prevHovered && hoveredAlgoIndex != -1) {
                        hoverSound.play();
                    }
                }
                
                if (currentState == SELECTING_COMPANY) {
                    int prevHovered = hoveredCompanyIndex;
                    hoveredCompanyIndex = -1;
                    for (int i = 0; i < companyBoxes.getSize(); i++) {
                        if (companyBoxes[i].getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                            hoveredCompanyIndex = i;
                            if (hoveredCompanyIndex != prevHovered && hoveredCompanyIndex != -1) {
                                hoverSound.play();
                            }
                            break;
                        }
                    }
                }
            }
        }
        
        // Update algorithm dropdown visual states
        if (currentState == SELECTING_PATHFINDING_ALGO) {
            for (int i = 0; i < algoBoxes.getSize(); i++) {
                if (i == hoveredAlgoIndex) {
                    algoBoxes[i].setOutlineColor(Color(0, 255, 255));
                    algoBoxes[i].setOutlineThickness(3);
                    algoBoxes[i].setFillColor(Color(30, 40, 60, 240));
                    algoTexts[i].setFillColor(Color(0, 255, 255));
                } else {
                    algoBoxes[i].setOutlineColor(Color(100, 100, 100));
                    algoBoxes[i].setOutlineThickness(2);
                    algoBoxes[i].setFillColor(Color(20, 25, 40, 220));
                    algoTexts[i].setFillColor(Color::White);
                }
            }
        } else {
            // Reset hover when not in algorithm selection
            hoveredAlgoIndex = -1;
        }
        
        // Update company dropdown visual states
        if (currentState == SELECTING_COMPANY) {
            for (int i = 0; i < companyBoxes.getSize(); i++) {
                if (i == hoveredCompanyIndex) {
                    companyBoxes[i].setOutlineColor(Color(150, 150, 150));
                    companyBoxes[i].setOutlineThickness(2);
                    companyBoxes[i].setFillColor(Color(40, 40, 50, 220));
                    companyTexts[i].setFillColor(Color(200, 200, 200));
                } else {
                    companyBoxes[i].setOutlineColor(Color(80, 80, 80));
                    companyBoxes[i].setOutlineThickness(1);
                    companyBoxes[i].setFillColor(Color(20, 20, 25, 200));
                    companyTexts[i].setFillColor(Color(150, 150, 150));
                }
            }
        } else {
            // Reset hover when not in company selection
            hoveredCompanyIndex = -1;
        }

        // Update Animations
        renderer.update(dt);
        
        // Hide warning after 5 seconds
        if (showWarning && warningClock.getElapsedTime().asSeconds() > 5.0f) {
            showWarning = false;
        }
        
        // Update cursor blinking
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
        
        // Update text box highlights based on current state - simplified
        if (currentState == IDLE || currentState == INPUTTING_SOURCE) {
            sourceBox.setOutlineColor(Color(120, 120, 120));
            sourceBox.setOutlineThickness(2);
            destBox.setOutlineColor(Color(80, 80, 80));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(80, 80, 80));
            dateBox.setOutlineThickness(1);
            avoidPortsBox.setOutlineColor(Color(80, 80, 80));
            avoidPortsBox.setOutlineThickness(1);
            maxTimeBox.setOutlineColor(Color(80, 80, 80));
            maxTimeBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_DEST) {
            sourceBox.setOutlineColor(Color(80, 80, 80));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(120, 120, 120));
            destBox.setOutlineThickness(2);
            dateBox.setOutlineColor(Color(80, 80, 80));
            dateBox.setOutlineThickness(1);
            avoidPortsBox.setOutlineColor(Color(80, 80, 80));
            avoidPortsBox.setOutlineThickness(1);
            maxTimeBox.setOutlineColor(Color(80, 80, 80));
            maxTimeBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_DEPARTURE_DATE) {
            sourceBox.setOutlineColor(Color(80, 80, 80));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(80, 80, 80));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(120, 120, 120));
            dateBox.setOutlineThickness(2);
            avoidPortsBox.setOutlineColor(Color(80, 80, 80));
            avoidPortsBox.setOutlineThickness(1);
            maxTimeBox.setOutlineColor(Color(80, 80, 80));
            maxTimeBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_AVOID_PORTS) {
            sourceBox.setOutlineColor(Color(80, 80, 80));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(80, 80, 80));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(80, 80, 80));
            dateBox.setOutlineThickness(1);
            avoidPortsBox.setOutlineColor(Color(0, 200, 255));
            avoidPortsBox.setOutlineThickness(2);
            maxTimeBox.setOutlineColor(Color(80, 80, 80));
            maxTimeBox.setOutlineThickness(1);
        } else if (currentState == INPUTTING_MAX_VOYAGE_TIME) {
            sourceBox.setOutlineColor(Color(80, 80, 80));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(80, 80, 80));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(80, 80, 80));
            dateBox.setOutlineThickness(1);
            avoidPortsBox.setOutlineColor(Color(80, 80, 80));
            avoidPortsBox.setOutlineThickness(1);
            maxTimeBox.setOutlineColor(Color(0, 200, 255));
            maxTimeBox.setOutlineThickness(2);
        } else {
            sourceBox.setOutlineColor(Color(80, 80, 80));
            sourceBox.setOutlineThickness(1);
            destBox.setOutlineColor(Color(80, 80, 80));
            destBox.setOutlineThickness(1);
            dateBox.setOutlineColor(Color(80, 80, 80));
            dateBox.setOutlineThickness(1);
            avoidPortsBox.setOutlineColor(Color(80, 80, 80));
            avoidPortsBox.setOutlineThickness(1);
            maxTimeBox.setOutlineColor(Color(80, 80, 80));
            maxTimeBox.setOutlineThickness(1);
        }

        window.clear(Color(5, 10, 20)); // Very dark blue background
        renderer.drawWorld(oceanGraph, selectedPort, showAllRoutes, sourcePort, destPort);
        
        // Draw HUD panel
        renderer.drawHUD(sourcePort, destPort, pathResult, oceanGraph, stateString, selectedAlgorithmName);
        
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
        
        // Draw placeholder text if fields are empty
        if (sourceInput.empty() && (currentState == IDLE || currentState == INPUTTING_SOURCE)) {
            window.draw(sourcePlaceholder);
        }
        if (destInput.empty() && currentState == INPUTTING_DEST) {
            window.draw(destPlaceholder);
        }
        if (departureDateInput.empty() && currentState == INPUTTING_DEPARTURE_DATE) {
            window.draw(datePlaceholder);
        }
        
        window.draw(sourceText);
        window.draw(destText);
        window.draw(dateText);
        
        // Draw blinking cursor
        if (showCursor) {
            if (currentState == IDLE || currentState == INPUTTING_SOURCE) {
                FloatRect sourceBounds = sourceText.getLocalBounds();
                cursor.setPosition(58 + sourceBounds.width, 56);
                window.draw(cursor);
            } else if (currentState == INPUTTING_DEST) {
                FloatRect destBounds = destText.getLocalBounds();
                cursor.setPosition(58 + destBounds.width, 116);
                window.draw(cursor);
            } else if (currentState == INPUTTING_DEPARTURE_DATE) {
                FloatRect dateBounds = dateText.getLocalBounds();
                cursor.setPosition(58 + dateBounds.width, 176);
                window.draw(cursor);
            } else if (currentState == INPUTTING_AVOID_PORTS) {
                FloatRect avoidBounds = avoidPortsText.getLocalBounds();
                cursor.setPosition(558 + avoidBounds.width, 406);
                window.draw(cursor);
            } else if (currentState == INPUTTING_MAX_VOYAGE_TIME) {
                FloatRect timeBounds = maxTimeText.getLocalBounds();
                cursor.setPosition(558 + timeBounds.width, 496);
                window.draw(cursor);
            }
        }
        
        // Draw main menu if in MAIN_MENU state
        if (currentState == MAIN_MENU) {
            renderer.drawMainMenu(false);
        }
        // Draw algorithm options menu ONLY when in options state
        else if (currentState == SELECTING_ALGO_OPTIONS || 
                 currentState == INPUTTING_AVOID_PORTS || 
                 currentState == INPUTTING_MAX_VOYAGE_TIME) {
            renderer.drawAlgorithmOptionsMenu(useAvoidPorts, useMaxVoyageTime, avoidPortsInput, maxVoyageTime);
            
            // Draw input boxes for options
            window.draw(avoidPortsBox);
            window.draw(avoidPortsLabel);
            if (avoidPortsStr.empty()) {
                window.draw(avoidPortsPlaceholder);
            } else {
                window.draw(avoidPortsText);
            }
            
            window.draw(maxTimeBox);
            window.draw(maxTimeLabel);
            if (maxVoyageTimeStr.empty()) {
                window.draw(maxTimePlaceholder);
            } else {
                window.draw(maxTimeText);
            }
        } else {
            // Normal game rendering
            
            // Draw company dropdown if in SELECTING_COMPANY state
            if (currentState == SELECTING_COMPANY) {
                // Draw semi-transparent background overlay
                RectangleShape overlay(Vector2f(1600, 900));
                overlay.setFillColor(Color(0, 0, 0, 150));
                window.draw(overlay);
                
                // Draw company boxes
                for (int i = 0; i < companyBoxes.getSize(); i++) {
                    window.draw(companyBoxes[i]);
                    window.draw(companyTexts[i]);
                }
            }
            
            window.draw(instructions);
        }
        
        // Draw instruction background panel
        if (currentState != MAIN_MENU && currentState != SELECTING_PATHFINDING_ALGO) {
            textBounds = instructions.getLocalBounds();
            instructionBg.setSize(Vector2f(textBounds.width + 20, 30));
            instructionBg.setPosition(800 - (textBounds.width + 20) / 2, 5);
            window.draw(instructionBg);
        }
        
        // Draw warning panel if active - simplified
        if (showWarning) {
            window.draw(warningBg);
            window.draw(warningText);
        }
        
        window.draw(instructions);
        window.display();
    }

    return 0;
}