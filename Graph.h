#pragma once
#include <iostream>
#include "Port.h"
#include "Route.h"
#include "Vector.h"
#include "DateTime.h"
#include "LinkedList.h"
#include "MinHeap.h"
#include "Ship.h"
#include "PathNode.h"
#include "PathResult.h"
#include <fstream>
using namespace std;

const int DockingTime = 2;
const double INF = 1e9;

struct Graph
{
    Vector<Port *> ports;

    bool isNextDay(int depHour, int depMinute, int arrHour, int arrMinute)
    {
        return arrHour < depHour || (arrHour == depHour && arrMinute < depMinute);
    }

    int findPortIndex(const string &portName)
    {
        // Convert search string to lowercase for case-insensitive comparison
        string searchName = portName;
        for (int i = 0; i <searchName.length(); i++) {
            searchName[i] = tolower(searchName[i]);
        }
        
        for (int i = 0; i < ports.getSize(); i++)
        {
            string currentName = ports[i]->name;
            for (int j = 0; j < currentName.length(); j++) {
                currentName[j] = tolower(currentName[j]);
            }
            
            if (currentName == searchName)
            {
                return i;
            }
        }
        return -1;
    }
    double freeTime(Port *port, Ship ship)
    {
        Queue<Ship> temp = port->shipsQueue;
        while (temp.front() != ship)
        {
            Ship currShip = temp.front();
            temp.dequeue();
            currShip.arrivalTime.hour += DockingTime;
            if (currShip.arrivalTime.hour >= 24)
            {
                currShip.arrivalTime.hour -= 24;
                currShip.arrivalTime.day++;
                if (currShip.arrivalTime.day > 31)
                {
                    currShip.arrivalTime.day = 1;
                    currShip.arrivalTime.month++;
                    if (currShip.arrivalTime.month > 12)
                    {
                        currShip.arrivalTime.month = 1;
                        currShip.arrivalTime.year++;
                    }
                }
            }
            if (currShip.arrivalTime.timeDiff(temp.front().arrivalTime) < 0)
            {
                temp.front().arrivalTime.hour += abs(temp.front().arrivalTime.timeDiff(currShip.arrivalTime));
                if (temp.front().arrivalTime.hour >= 24)
                {
                    temp.front().arrivalTime.hour -= 24;
                    temp.front().arrivalTime.day++;
                    if (temp.front().arrivalTime.day > 31)
                    {
                        temp.front().arrivalTime.day = 1;
                        temp.front().arrivalTime.month++;
                        if (temp.front().arrivalTime.month > 12)
                        {
                            temp.front().arrivalTime.month = 1;
                            temp.front().arrivalTime.year++;
                        }
                    }
                }
            }
        }
        return ship.arrivalTime.timeDiff(temp.front().arrivalTime) + DockingTime;
    }
    void createRoutes()
    {
        ifstream file("Routes.txt");
        if (!file.is_open())
        {
            cerr << "ERROR: Cannot open Routes.txt" << endl;
            return;
        }
        string line;
        int portCount = ports.getSize();
        Vector<MinHeap<Ship>> heaps;
        for (int i = 0; i < portCount; i++) {
            heaps.push_back(MinHeap<Ship>());
        }
        while (getline(file, line))
        {
            string sourcePortName = line.substr(0, line.find(" "));
            line = line.substr(line.find(" ") + 1);
            string destinationPortName = line.substr(0, line.find(" "));
            line = line.substr(line.find(" ") + 1);
            string Date = line.substr(0, line.find(" "));
            line = line.substr(line.find(" ") + 1);
            string departureTimeStr = line.substr(0, line.find(" "));
            line = line.substr(line.find(" ") + 1);
            string arrivalTimeStr = line.substr(0, line.find(" "));
            line = line.substr(line.find(" ") + 1);
            double voyageCost = stod(line.substr(0, line.find(" ")));
            line = line.substr(line.find(" ") + 1);
            string company = line;

            int day = stoi(Date.substr(0, Date.find("/")));
            Date = Date.substr(Date.find("/") + 1);
            int month = stoi(Date.substr(0, Date.find("/")));
            Date = Date.substr(Date.find("/") + 1);
            int year = stoi(Date);

            int departureHour = stoi(departureTimeStr.substr(0, departureTimeStr.find(":")));
            departureTimeStr = departureTimeStr.substr(departureTimeStr.find(":") + 1);
            int departureMinute = stoi(departureTimeStr);
            int arrivalHour = stoi(arrivalTimeStr.substr(0, arrivalTimeStr.find(":")));
            arrivalTimeStr = arrivalTimeStr.substr(arrivalTimeStr.find(":") + 1);
            int arrivalMinute = stoi(arrivalTimeStr);

            bool nextDay = isNextDay(departureHour, departureMinute, arrivalHour, arrivalMinute);
            bool nextMonth = ((nextDay) ? day + 1 : day) > 31;
            bool nextYear = ((nextMonth) ? month + 1 : month) > 12;

            DateTime departureTime(year, month, day, departureHour, departureMinute);
            DateTime arrivalTime(nextYear ? year + 1 : year, 
                               nextMonth ? (month + 1 > 12 ? 1 : month + 1) : month, 
                               nextDay ? (day + 1 > 31 ? 1 : day + 1) : day, 
                               arrivalHour, arrivalMinute);

            int sourceIndex = -1;
            int destinationIndex = -1;
            sourceIndex = findPortIndex(sourcePortName);
            destinationIndex = findPortIndex(destinationPortName);
            if (sourceIndex != -1 && destinationIndex != -1)
            {
                Route route(sourcePortName, destinationPortName, destinationIndex, departureTime, arrivalTime, voyageCost, company);
                ports[sourceIndex]->routes.insertAtHead(route);
                Ship temp(ports[sourceIndex]->routes.head->data.arrivalTime, company, destinationPortName, sourcePortName);
                heaps[destinationIndex].push(temp);
            }
            else
            {
                cout << "Error: Invalid port name in route data." << endl;
            }
        }
        file.close();

        for (int i = 0; i < ports.getSize(); i++)
        {
            while (!heaps[i].isEmpty())
            {
                ports[i]->shipsQueue.enqueue(heaps[i].top());
                heaps[i].pop();
            }
        }
    }
    void createPorts()
    {
        ifstream file("PortCharges.txt");
        if (!file.is_open())
        {
            cerr << "ERROR: Cannot open PortCharges.txt" << endl;
            return;
        }
        string line;
        while (getline(file, line))
        {
            string name = line.substr(0, line.find(" "));
            double portCharges = stod(line.substr(line.find(" ") + 1));
            Port *port = new Port(name, portCharges);
            ports.push_back(port);
        }
        file.close();
    }
    // void displayGraph()
    // {
    //     for (int i = 0; i < ports.getSize(); i++)
    //     {
    //         Node<Route> *current = ports[i]->routes.head;
    //         cout << "Port: " << ports[i]->name << ", Charges: " << ports[i]->portCharges << endl;
    //         while (current != nullptr)
    //         {
    //             current->data.displayRoute();
    //             cout<< "Destination port name : " << ports[current->data.destinationIndex]->name << endl<<endl;
    //             current = current->next;
    //         }
    //     }
    // }
    
    
    // Dijkstra's algorithm for shortest path
    // optimizeTime: true = minimize time, false = minimize cost
    // companyFilter: empty string = all companies, otherwise only use routes from this company
    PathResult findPathDijkstra(int sourceIdx, int destIdx, DateTime startTime, bool optimizeTime = false, string companyFilter = "")
    {
        PathResult result;
        
        // Validate inputs
        if (sourceIdx < 0 || sourceIdx >= ports.getSize() || 
            destIdx < 0 || destIdx >= ports.getSize()) {
            cout << "Error: Invalid port indices" << endl;
            return result;
        }
        
        if (sourceIdx == destIdx) {
            cout << "Error: Source and destination are the same" << endl;
            return result;
        }
        
        int n = ports.getSize();
        
        // Initialize data structures using custom arrays
        Vector<double> dist;
        Vector<int> parent;
        Vector<Route*> parentRoute;
        Vector<DateTime> arrivalTime;
        Vector<bool> visited;
        
        // Initialize arrays with proper sizes
        for (int i = 0; i < n; i++) {
            dist.push_back(INF);
            parent.push_back(-1);
            parentRoute.push_back(nullptr);
            arrivalTime.push_back(DateTime());
            visited.push_back(false);
        }
        
        // Set source
        dist[sourceIdx] = 0.0;
        arrivalTime[sourceIdx] = startTime;
        
        // Priority queue for Dijkstra
        MinHeap<PathNode> pq;
        pq.push(PathNode(0.0, sourceIdx));
        
        cout << "\n====== DIJKSTRA EXPLORATION LOG ======" << endl;
        cout << "Source: " << ports[sourceIdx]->name << endl;
        cout << "Destination: " << ports[destIdx]->name << endl;
        cout << "Company Filter: " << (companyFilter.empty() ? "ALL COMPANIES" : companyFilter) << endl;
        cout << "Optimization: " << (optimizeTime ? "TIME (minimize hours)" : "COST (minimize $)") << endl;
        cout << "======================================\n" << endl;
        
        // Main Dijkstra loop
        int iteration = 0;
        while (!pq.isEmpty()) {
            PathNode current = pq.top();
            pq.pop();
            
            int currentPort = current.portIndex;
            
            // Skip if already visited
            if (visited[currentPort]) {
                cout << "  [SKIP] " << ports[currentPort]->name << " (already visited)" << endl;
                continue;
            }
            
            visited[currentPort] = true;
            result.nodesExplored++;
            iteration++;
            
            // Record exploration order for visualization
            result.explorationOrder.push_back(currentPort);
            
            // Log exploration
            cout << "\n--- Iteration " << iteration << " ---" << endl;
            cout << "EXPLORING: " << ports[currentPort]->name;
            if (optimizeTime) {
                cout << " (accumulated time: " << current.cost << " hours)" << endl;
            } else {
                cout << " (accumulated cost: $" << current.cost << ")" << endl;
            }
            
            // Early exit if we reached destination
            if (currentPort == destIdx) {
                result.pathFound = true;
                cout << "\n[DEST] DESTINATION REACHED! " << ports[destIdx]->name << endl;
                if (optimizeTime) {
                    cout << "   Final time: " << current.cost << " hours" << endl;
                } else {
                    cout << "   Final cost: $" << current.cost << endl;
                }
                break;
            }
            
            // Explore all outgoing routes from current port
            Node<Route>* routeNode = ports[currentPort]->routes.head;
            
            cout << "  Checking neighbors of " << ports[currentPort]->name << ":" << endl;
            int routeCount = 0;
            
            while (routeNode != nullptr) {
                Route& route = routeNode->data;
                int nextPort = route.destinationIndex;
                
                // Skip if already visited
                if (visited[nextPort]) {
                    cout << "    - " << ports[nextPort]->name << " [SKIP: already visited]" << endl;
                    routeNode = routeNode->next;
                    continue;
                }
                
                // TEMPORAL FEASIBILITY CHECK: Can we catch this ship?
                // We can only board if the ship departs AFTER we arrive at current port
                DateTime ourArrivalAtCurrentPort = arrivalTime[currentPort];
                if (route.departureTime < ourArrivalAtCurrentPort) {
                    cout << "    - " << ports[nextPort]->name << " [SKIP: ship departs ";
                    cout << route.departureTime.day << "/" << route.departureTime.month << " " 
                         << route.departureTime.hour << ":" << route.departureTime.minute;
                    cout << " BEFORE we arrive at " << ports[currentPort]->name << " on ";
                    cout << ourArrivalAtCurrentPort.day << "/" << ourArrivalAtCurrentPort.month << " "
                         << ourArrivalAtCurrentPort.hour << ":" << ourArrivalAtCurrentPort.minute << "]" << endl;
                    routeNode = routeNode->next;
                    continue;
                }
                
                if (!companyFilter.empty() && route.company != companyFilter) {
                    cout << "    - " << ports[nextPort]->name << " [SKIP: company " << route.company << " != " << companyFilter << "]" << endl;
                    routeNode = routeNode->next;
                    continue;
                }
                
                double edgeWeight = 0.0;
                double waitHours = 0.0;
                
                // Create ship for queue simulation
                Ship arrivalShip(
                    route.arrivalTime,
                    route.company,
                    route.destinationPortName,
                    route.sourcePortName
                );
                
                // Calculate wait time at destination port
                waitHours = freeTime(ports[nextPort], arrivalShip);
                
                if (optimizeTime) {
                    double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                    edgeWeight = voyageHours + waitHours;
                }
                else {
                    double voyageCost = route.voyageCost;
                    double layoverCharges = 0.0;
                    
                    // Add port charges if layover exceeds 12 hours
                    if (waitHours > 12.0) {
                        layoverCharges = ports[nextPort]->portCharges * (waitHours / 24.0);
                    }
                    
                    edgeWeight = voyageCost + layoverCharges;
                }
                
                double newDist = dist[currentPort] + edgeWeight;
                
                cout << "    → " << ports[nextPort]->name << " [" << route.company << "]";
                
                if (optimizeTime) {
                    cout << " voyage=" << route.departureTime.timeDiff(route.arrivalTime) << "h";
                    cout << " wait=" << waitHours << "h";
                    cout << " total=" << newDist << "h";
                } else {
                    cout << " cost=$" << route.voyageCost;
                    if (waitHours > 12.0) {
                        double layoverFee = ports[nextPort]->portCharges * (waitHours / 24.0);
                        cout << " layover=$" << layoverFee;
                    }
                    cout << " total=$" << newDist;
                }
                
                if (newDist < dist[nextPort]) {
                    dist[nextPort] = newDist;
                    parent[nextPort] = currentPort;
                    parentRoute[nextPort] = &(routeNode->data);  // Store the route pointer
                    
                    // Update arrival time at next port
                    arrivalTime[nextPort] = route.arrivalTime.addHours(waitHours);
                    
                    // Add to priority queue
                    pq.push(PathNode(newDist, nextPort));
                    
                    cout << " [OK] BETTER PATH (was ";
                    if (dist[nextPort] >= INF - 1) {
                        cout << "∞";
                    } else {
                        if (optimizeTime) {
                            cout << dist[nextPort] << "h";
                        } else {
                            cout << "$" << dist[nextPort];
                        }
                    }
                    cout << ")" << endl;
                    routeCount++;
                } else {
                    cout << " [ERROR] worse than current ";
                    if (optimizeTime) {
                        cout << dist[nextPort] << "h" << endl;
                    } else {
                        cout << "$" << dist[nextPort] << endl;
                    }
                }
                
                routeNode = routeNode->next;
            }
            
            if (routeCount > 0) {
                cout << "  > Added " << routeCount << " port(s) to exploration queue" << endl;
            }
        }
        
        cout << "\n====== EXPLORATION COMPLETE ======" << endl;
        cout << "Total ports explored: " << result.nodesExplored << " / " << ports.getSize() << endl;
        cout << "==================================\n" << endl;
        
        if (!result.pathFound) {
            cout << "[FAIL] No path found from " << ports[sourceIdx]->name 
                 << " to " << ports[destIdx]->name << endl;
            return result;
        }
        
        cout << "\n====== PATH RECONSTRUCTION ======" << endl;
        cout << "Backtracking from " << ports[destIdx]->name << " to " << ports[sourceIdx]->name << "..." << endl;
        
        // Build path from destination back to source using stored routes
        int current = destIdx;
        Vector<Route> pathRoutes;  // Temporary vector to reverse the path
        
        // Record final path ports for visualization
        result.finalPathPorts.push_back(destIdx);
        
        int hopNum = 1;
        while (parent[current] != -1) {
            int prev = parent[current];
            Route* route = parentRoute[current];
            
            if (route == nullptr) {
                cout << "Error: Route tracking failed during reconstruction" << endl;
                result.pathFound = false;
                return result;
            }
            
            cout << "  Hop " << hopNum << " (backwards): " << ports[prev]->name 
                 << " → " << ports[current]->name 
                 << " [" << route->company << "] $" << route->voyageCost << endl;
            
            // Record port in final path
            result.finalPathPorts.push_back(prev);
            
            // Store route for reversal
            pathRoutes.push_back(*route);
            
            // Accumulate costs and times
            result.totalCost += route->voyageCost;
            result.totalTime += route->departureTime.timeDiff(route->arrivalTime);  // FIX: departure.timeDiff(arrival) for positive time
            
            // Add layover charges if applicable
            Ship ship(route->arrivalTime, 
                     route->company,
                     route->destinationPortName,
                     route->sourcePortName);
            double wait = freeTime(ports[current], ship);
            if (wait > 12.0) {
                double layoverFee = ports[current]->portCharges * (wait / 24.0);
                result.totalCost += layoverFee;
                cout << "    + Layover charge at " << ports[current]->name 
                     << ": $" << layoverFee << " (wait: " << wait << "h)" << endl;
            }
            result.totalTime += wait;
            
            current = prev;
            hopNum++;
        }
        
        cout << "\n[OK] Path reconstructed successfully!" << endl;
        cout << "  Total hops: " << pathRoutes.getSize() << endl;
        cout << "  Total cost: $" << result.totalCost << endl;
        cout << "  Total time: " << result.totalTime << " hours (" 
             << (result.totalTime / 24.0) << " days)" << endl;
        cout << "=================================\n" << endl;
        
        // Reverse the path and add to result (since we built it backwards)
        for (int i = pathRoutes.getSize() - 1; i >= 0; i--) {
            result.routes.insertAtEnd(pathRoutes[i]);
        }
        
        return result;
    }
    
    // Placeholder for A* (to be implemented later)
    PathResult findPathAStar(int sourceIdx, int destIdx, DateTime startTime, bool optimizeTime = false)
    {
        return findPathDijkstra(sourceIdx, destIdx, startTime, optimizeTime);
    }
    
    Graph()
    {
        createPorts();
        createRoutes();
    }
    
    ~Graph()
    {
        for (int i = 0; i < ports.getSize(); i++)
        {
            delete ports[i];
        }
    }
};