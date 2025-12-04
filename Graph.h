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
#include <cmath>
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
        for (int i = 0; i < searchName.length(); i++)
        {
            searchName[i] = tolower(searchName[i]);
        }

        for (int i = 0; i < ports.getSize(); i++)
        {
            string currentName = ports[i]->name;
            for (int j = 0; j < currentName.length(); j++)
            {
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
        DateTime portFreeAt;
        bool initialized = false;

        while (!temp.isEmpty() && temp.front() != ship)
        {
            Ship currShip = temp.front();
            temp.dequeue();

            // When can this ship start docking?
            DateTime dockStartTime = currShip.arrivalTime;
            if (initialized && portFreeAt > dockStartTime)
            {
                dockStartTime = portFreeAt;
            }

            // This ship finishes docking after DockingTime hours
            portFreeAt = dockStartTime.addHours(DockingTime);
            initialized = true;
        }

        // Calculate when we can start docking
        DateTime ourDockStart = ship.arrivalTime;
        if (initialized && portFreeAt > ourDockStart)
        {
            ourDockStart = portFreeAt;
        }

        // Return total time at port: wait time + docking time
        double waitTime = ship.arrivalTime.timeDiff(ourDockStart);
        return waitTime + DockingTime;
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
        for (int i = 0; i < portCount; i++)
        {
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

    void loadCoordinates()
    {
        ifstream file("Coordinates.txt");
        if (!file.is_open())
        {
            cerr << "ERROR: Cannot open Coordinates.txt" << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            // Parse: PortName Latitude Longitude
            int firstSpace = line.find(' ');
            if (firstSpace == -1)
                continue;

            string portName = line.substr(0, firstSpace);

            int secondSpace = line.find(' ', firstSpace + 1);
            if (secondSpace == -1)
                continue;

            string latStr = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            string lonStr = line.substr(secondSpace + 1);

            double lat = stod(latStr);
            double lon = stod(lonStr);

            for (int i = 0; i < ports.getSize(); i++)
            {
                if (ports[i]->name == portName)
                {
                    ports[i]->latitude = lat;
                    ports[i]->longitude = lon;
                    break;
                }
            }
        }
        file.close();
    }

    double euclideanDistance(int port1Idx, int port2Idx)
    {
        double lat1 = ports[port1Idx]->latitude;
        double lon1 = ports[port1Idx]->longitude;
        double lat2 = ports[port2Idx]->latitude;
        double lon2 = ports[port2Idx]->longitude;

        double dLat = lat2 - lat1;
        double dLon = lon2 - lon1;

        double distance = sqrt(dLat * dLat + dLon * dLon);

        return distance;
    }

    void DFSfindPaths(int currentIdx, int destIdx, DateTime availableTime,
                      Vector<bool> &visited, LinkedList<Route> &currentPath,
                      AllPathsResult &result, int &nodesVisited, DateTime startTime,
                      Vector<string> avoidPorts = Vector<string>(), double maxVoyageTime = -1)
    {
        nodesVisited++;

        if (currentIdx == destIdx)
        {
            // Create a PathResult for this complete path
            PathResult pathResult;
            pathResult.pathFound = true;
            pathResult.routes = currentPath;

            // Calculate total cost and time by traversing the complete path
            double totalCost = 0.0;
            double totalTime = 0.0;
            Node<Route> *routeNode = currentPath.head;
            Route *nextRoute = nullptr;

            while (routeNode)
            {
                Route &route = routeNode->data;

                // Add voyage cost
                totalCost += route.voyageCost;

                // Calculate voyage time
                double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                totalTime += voyageHours;

                // Calculate queue + dock time at destination port
                Ship tempShip(route.arrivalTime, route.company,
                              route.destinationPortName, route.sourcePortName);
                double queueAndDock = freeTime(ports[route.destinationIndex], tempShip);

                // Layover charges based on queue + dock time only
                if (queueAndDock > 12.0)
                {
                    totalCost += ports[route.destinationIndex]->portCharges * (queueAndDock / 24.0);
                }

                // Add queue and dock time to total time
                totalTime += queueAndDock;

                // If there's a next route, add wait time from free until next departure
                if (routeNode->next != nullptr)
                {
                    nextRoute = &(routeNode->next->data);
                    DateTime freeTime_dt = route.arrivalTime.addHours(queueAndDock);
                    double waitForNextDeparture = freeTime_dt.timeDiff(nextRoute->departureTime);
                    totalTime += waitForNextDeparture;
                }

                routeNode = routeNode->next;
            }

            pathResult.totalCost = totalCost;
            pathResult.totalTime = totalTime;

            // Check max voyage time limit for complete path BEFORE adding
            if (maxVoyageTime >= 0 && totalTime > maxVoyageTime)
            {
                return;
            }

            result.allPaths.push_back(pathResult);
            result.totalPathsFound++;
            return;
        }

        Node<Route> *routeNode = ports[currentIdx]->routes.head;
        while (routeNode)
        {
            Route &currRoute = routeNode->data;
            int nextIdx = currRoute.destinationIndex;
            if (!visited[nextIdx])
            {
                if (currRoute.departureTime < availableTime)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                Ship tempShip(currRoute.arrivalTime, currRoute.company,
                              currRoute.destinationPortName, currRoute.sourcePortName);
                double waitDock = freeTime(ports[nextIdx], tempShip);
                DateTime nextAvailable = currRoute.arrivalTime.addHours(waitDock);

                // Check if next port is in avoid list
                bool shouldAvoid = false;
                for (int i = 0; i < avoidPorts.getSize(); i++)
                {
                    if (ports[nextIdx]->name == avoidPorts[i])
                    {
                        shouldAvoid = true;
                        break;
                    }
                }

                if (shouldAvoid)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Note: Max voyage time is checked on complete paths only (at destination)
                // Not checked here during exploration to allow full path discovery

                visited[nextIdx] = true;
                currentPath.insertAtEnd(currRoute);
                DFSfindPaths(nextIdx, destIdx, nextAvailable, visited, currentPath, result, nodesVisited, startTime, avoidPorts, maxVoyageTime);
                visited[nextIdx] = false;
                currentPath.popAtEnd();
            }

            routeNode = routeNode->next;
        }
    }

    AllPathsResult findAllPaths(string source, string destination, DateTime start,
                                Vector<string> avoidPorts = Vector<string>(), double maxVoyageTime = -1)
    {
        int srcIdx = findPortIndex(source);
        int destIdx = findPortIndex(destination);
        AllPathsResult result;

        if (srcIdx == -1 || destIdx == -1)
        {
            return result;
        }

        // Same source and destination - no valid path
        if (srcIdx == destIdx)
        {
            return result;
        }

        Vector<bool> visited(ports.getSize());
        for (int i = 0; i < ports.getSize(); i++)
        {
            visited.push_back(false);
        }
        LinkedList<Route> currentPath;
        int nodesVisited = 0;

        DFSfindPaths(srcIdx, destIdx, start, visited, currentPath, result, nodesVisited, start, avoidPorts, maxVoyageTime);
        result.nodesExplored = nodesVisited;

        return result;
    }

    // Dijkstra's algorithm for shortest path
    // optimizeTime: true = minimize time, false = minimize cost
    // companyFilter: empty string = all companies, otherwise only use routes from this company
    // avoidPorts: list of port names to exclude from path
    // maxVoyageTime: maximum total voyage time allowed (-1 = no limit)
    PathResult findPathDijkstra(int sourceIdx, int destIdx, DateTime startTime, bool optimizeTime = false,
                                string companyFilter = "", Vector<string> avoidPorts = Vector<string>(),
                                double maxVoyageTime = -1)
    {
        PathResult result;

        // Validate inputs
        if (sourceIdx < 0 || sourceIdx >= ports.getSize() ||
            destIdx < 0 || destIdx >= ports.getSize())
        {
            cout << "Error: Invalid port indices" << endl;
            return result;
        }

        if (sourceIdx == destIdx)
        {
            cout << "Error: Source and destination are the same" << endl;
            return result;
        }

        int n = ports.getSize();

        // Initialize data structures using custom arrays
        Vector<double> dist;
        Vector<double> timeElapsed; // Track actual time regardless of optimization mode
        Vector<int> parent;
        Vector<Route *> parentRoute;
        Vector<DateTime> arrivalTime;
        Vector<bool> visited;

        // Initialize arrays with proper sizes
        for (int i = 0; i < n; i++)
        {
            dist.push_back(INF);
            timeElapsed.push_back(INF);
            parent.push_back(-1);
            parentRoute.push_back(nullptr);
            arrivalTime.push_back(DateTime());
            visited.push_back(false);
        }

        // Set source
        dist[sourceIdx] = 0.0;
        timeElapsed[sourceIdx] = 0.0;
        arrivalTime[sourceIdx] = startTime;

        // Priority queue for Dijkstra
        MinHeap<PathNode> pq;
        pq.push(PathNode(0.0, sourceIdx));

        // Main Dijkstra loop
        int iteration = 0;
        while (!pq.isEmpty())
        {
            iteration++;
            PathNode current = pq.top();
            pq.pop();

            int currentPort = current.portIndex;

            // Skip if already visited
            if (visited[currentPort])
            {
                continue;
            }

            visited[currentPort] = true;
            result.nodesExplored++;

            // Record exploration order for visualization
            result.explorationOrder.push_back(currentPort);

            // Early exit if we reached destination
            if (currentPort == destIdx)
            {
                // Check max voyage time limit at destination
                if (maxVoyageTime >= 0 && timeElapsed[destIdx] > maxVoyageTime)
                {
                    result.pathFound = false;
                    return result;
                }
                result.pathFound = true;
                break;
            }
            // Explore all outgoing routes from current port
            Node<Route> *routeNode = ports[currentPort]->routes.head;
            int routeCount = 0;

            while (routeNode != nullptr)
            {
                routeCount++;
                Route &route = routeNode->data;
                int nextPort = route.destinationIndex;

                // Skip if already visited
                if (visited[nextPort])
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Check if next port is in avoid list
                bool shouldAvoid = false;
                for (int i = 0; i < avoidPorts.getSize(); i++)
                {
                    if (ports[nextPort]->name == avoidPorts[i])
                    {
                        shouldAvoid = true;
                        break;
                    }
                }

                if (shouldAvoid)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Temporal feasibility: Can we catch this ship?
                DateTime ourArrivalAtCurrentPort = arrivalTime[currentPort];
                if (route.departureTime < ourArrivalAtCurrentPort)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Company filter check
                if (!companyFilter.empty() && route.company != companyFilter)
                {
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
                    route.sourcePortName);

                // Calculate wait time at destination port
                waitHours = freeTime(ports[nextPort], arrivalShip);

                if (optimizeTime)
                {
                    double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                    double departureAtNextPort = 0.0;
                    if (nextPort != destIdx)
                    {
                        
                    }
                    edgeWeight = voyageHours + waitHours + departureAtNextPort;
                }
                else
                {
                    double layoverFee = 0.0;
                    if (waitHours > 12.0)
                    {
                        layoverFee = ports[nextPort]->portCharges * (waitHours / 24.0);
                    }
                    edgeWeight = route.voyageCost + layoverFee;
                }

                double newDist = dist[currentPort] + edgeWeight;

                // Calculate actual time elapsed (regardless of optimization mode)
                double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                double newTimeElapsed = timeElapsed[currentPort] + voyageHours + waitHours;

                // Note: Max voyage time is NOT checked during exploration
                // It's checked only at destination to avoid pruning valid paths

                if (newDist < dist[nextPort])
                {
                    dist[nextPort] = newDist;
                    timeElapsed[nextPort] = newTimeElapsed;
                    parent[nextPort] = currentPort;
                    parentRoute[nextPort] = &(routeNode->data);

                    // Update arrival time at next port
                    arrivalTime[nextPort] = route.arrivalTime.addHours(waitHours);

                    // Add to priority queue
                    pq.push(PathNode(newDist, nextPort));
                }

                routeNode = routeNode->next;
            }
        }

        if (!result.pathFound)
        {
            return result;
        }

        // Build path from destination back to source using stored routes
        int current = destIdx;
        Vector<Route> pathRoutes;

        // Record final path ports for visualization
        result.finalPathPorts.push_back(destIdx);

        Route *nextRoute = nullptr; // Track next route for calculating wait time

        if (parent[current] != -1 && !optimizeTime)
        {
            result.totalCost += dist[current];
        }
        while (parent[current] != -1)
        {
            int prev = parent[current];
            Route *route = parentRoute[current];

            if (route == nullptr)
            {
                result.pathFound = false;
                return result;
            }

            // Record port in final path
            result.finalPathPorts.push_back(prev);

            // Store route for reversal
            pathRoutes.push_back(*route);

            // Accumulate voyage cost and time

            if (optimizeTime)
            {
                result.totalCost += route->voyageCost;
            }
            result.totalTime += route->departureTime.timeDiff(route->arrivalTime);

            // Calculate time at current port
            Ship ship(route->arrivalTime,
                      route->company,
                      route->destinationPortName,
                      route->sourcePortName);
            double queueAndDock = freeTime(ports[current], ship);

            // Layover charges based on queue + dock time only
            if (optimizeTime && queueAndDock > 12.0)
            {
                double layoverFee = ports[current]->portCharges * (queueAndDock / 24.0);
                result.totalCost += layoverFee;
            }

            // Add queue and dock time to total time
            result.totalTime += queueAndDock;

            // If there's a next route, add wait time from free until next departure
            if (nextRoute != nullptr)
            {
                DateTime freeTime_dt = route->arrivalTime.addHours(queueAndDock);
                double waitForNextDeparture = freeTime_dt.timeDiff(nextRoute->departureTime);
                result.totalTime += waitForNextDeparture;
            }

            nextRoute = route; // Current route becomes next route for previous hop
            current = prev;
        }

        // Reverse the path and add to result (since we built it backwards)
        for (int i = pathRoutes.getSize() - 1; i >= 0; i--)
        {
            result.routes.insertAtEnd(pathRoutes[i]);
        }

        return result;
    }

    // A* algorithm for shortest path with heuristic
    // optimizeTime: true = minimize time, false = minimize cost
    // companyFilter: empty string = all companies, otherwise only use routes from this company
    // avoidPorts: list of port names to exclude from path
    // maxVoyageTime: maximum total voyage time allowed (-1 = no limit)
    PathResult findPathAStar(int sourceIdx, int destIdx, DateTime startTime, bool optimizeTime = false,
                             string companyFilter = "", Vector<string> avoidPorts = Vector<string>(),
                             double maxVoyageTime = -1)
    {
        PathResult result;

        // Validate inputs
        if (sourceIdx < 0 || sourceIdx >= ports.getSize() ||
            destIdx < 0 || destIdx >= ports.getSize())
        {
            cout << "Error: Invalid port indices" << endl;
            return result;
        }

        if (sourceIdx == destIdx)
        {
            cout << "Error: Source and destination are the same" << endl;
            return result;
        }

        int n = ports.getSize();

        // Pre-calculate heuristic for all ports to destination
        Vector<double> heuristic;
        for (int i = 0; i < n; i++)
        {
            heuristic.push_back(euclideanDistance(i, destIdx));
        }

        // Initialize data structures using custom arrays
        Vector<double> dist;
        Vector<double> timeElapsed; // Track actual time regardless of optimization mode
        Vector<int> parent;
        Vector<Route *> parentRoute;
        Vector<DateTime> arrivalTime;
        Vector<bool> visited;

        // Initialize arrays with proper sizes
        for (int i = 0; i < n; i++)
        {
            dist.push_back(INF);
            timeElapsed.push_back(INF);
            parent.push_back(-1);
            parentRoute.push_back(nullptr);
            arrivalTime.push_back(DateTime());
            visited.push_back(false);
        }

        // Set source
        dist[sourceIdx] = 0.0;
        timeElapsed[sourceIdx] = 0.0;
        arrivalTime[sourceIdx] = startTime;

        // Priority queue for A* (using dist + heuristic)
        MinHeap<PathNode> pq;
        pq.push(PathNode(heuristic[sourceIdx], sourceIdx));

        // Main A* loop
        int iteration = 0;
        while (!pq.isEmpty())
        {
            iteration++;
            PathNode current = pq.top();
            pq.pop();

            int currentPort = current.portIndex;

            // Skip if already visited
            if (visited[currentPort])
            {
                continue;
            }

            visited[currentPort] = true;
            result.nodesExplored++;

            // Record exploration order for visualization
            result.explorationOrder.push_back(currentPort);

            // Early exit if we reached destination
            if (currentPort == destIdx)
            {
                // Check max voyage time limit at destination
                if (maxVoyageTime >= 0 && timeElapsed[destIdx] > maxVoyageTime)
                {
                    result.pathFound = false;
                    return result;
                }
                result.pathFound = true;
                break;
            }

            // Explore all outgoing routes from current port
            Node<Route> *routeNode = ports[currentPort]->routes.head;
            int routeCount = 0;

            while (routeNode != nullptr)
            {
                routeCount++;
                Route &route = routeNode->data;
                int nextPort = route.destinationIndex;

                // Skip if already visited
                if (visited[nextPort])
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Check if next port is in avoid list
                bool shouldAvoid = false;
                for (int i = 0; i < avoidPorts.getSize(); i++)
                {
                    if (ports[nextPort]->name == avoidPorts[i])
                    {
                        shouldAvoid = true;
                        break;
                    }
                }

                if (shouldAvoid)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Temporal feasibility: Can we catch this ship?
                DateTime ourArrivalAtCurrentPort = arrivalTime[currentPort];
                if (route.departureTime < ourArrivalAtCurrentPort)
                {
                    routeNode = routeNode->next;
                    continue;
                }

                // Company filter check
                if (!companyFilter.empty() && route.company != companyFilter)
                {
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
                    route.sourcePortName);

                // Calculate wait time at destination port
                waitHours = freeTime(ports[nextPort], arrivalShip);

                if (optimizeTime)
                {
                    double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                    edgeWeight = voyageHours + waitHours;
                }
                else
                {
                    double layoverFee = 0.0;
                    if (waitHours > 12.0)
                    {
                        layoverFee = ports[nextPort]->portCharges * (waitHours / 24.0);
                    }
                    edgeWeight = route.voyageCost + layoverFee;
                }

                double newDist = dist[currentPort] + edgeWeight;

                // Calculate actual time elapsed (regardless of optimization mode)
                double voyageHours = route.departureTime.timeDiff(route.arrivalTime);
                double newTimeElapsed = timeElapsed[currentPort] + voyageHours + waitHours;

                // Note: Max voyage time is NOT checked during exploration
                // It's checked only at destination to avoid pruning valid paths

                if (newDist < dist[nextPort])
                {
                    dist[nextPort] = newDist;
                    timeElapsed[nextPort] = newTimeElapsed;
                    parent[nextPort] = currentPort;
                    parentRoute[nextPort] = &(routeNode->data);

                    // Update arrival time at next port
                    arrivalTime[nextPort] = route.arrivalTime.addHours(waitHours);

                    // Add to priority queue with dist + heuristic for A*
                    pq.push(PathNode(newDist + heuristic[nextPort], nextPort));
                }

                routeNode = routeNode->next;
            }
        }

        if (!result.pathFound)
        {
            return result;
        }

        // Build path from destination back to source using stored routes
        int current = destIdx;
        Vector<Route> pathRoutes;

        // Record final path ports for visualization
        result.finalPathPorts.push_back(destIdx);

        Route *nextRoute = nullptr; // Track next route for calculating wait time

        if ( parent[current] != -1 && !optimizeTime )
        {
            result.totalCost += dist[current];
        }
        while (parent[current] != -1)
        {
            int prev = parent[current];
            Route *route = parentRoute[current];

            if (route == nullptr)
            {
                result.pathFound = false;
                return result;
            }

            // Record port in final path
            result.finalPathPorts.push_back(prev);

            // Store route for reversal
            pathRoutes.push_back(*route);

            // Accumulate voyage cost and time

            if (optimizeTime)
            {
                result.totalCost += route->voyageCost;
            }
            result.totalTime += route->departureTime.timeDiff(route->arrivalTime);

            // Calculate time at current port
            Ship ship(route->arrivalTime,
                      route->company,
                      route->destinationPortName,
                      route->sourcePortName);
            double queueAndDock = freeTime(ports[current], ship);

            // Layover charges based on queue + dock time only
            if (optimizeTime && queueAndDock > 12.0)
            {
                double layoverFee = ports[current]->portCharges * (queueAndDock / 24.0);
                result.totalCost += layoverFee;
            }

            // Add queue and dock time to total time
            result.totalTime += queueAndDock;

            // If there's a next route, add wait time from free until next departure
            if (nextRoute != nullptr)
            {
                DateTime freeTime_dt = route->arrivalTime.addHours(queueAndDock);
                double waitForNextDeparture = freeTime_dt.timeDiff(nextRoute->departureTime);
                result.totalTime += waitForNextDeparture;
            }

            nextRoute = route; // Current route becomes next route for previous hop
            current = prev;
        }

        // Reverse the path and add to result (since we built it backwards)
        for (int i = pathRoutes.getSize() - 1; i >= 0; i--)
        {
            result.routes.insertAtEnd(pathRoutes[i]);
        }

        return result;
    }

    Graph()
    {
        createPorts();
        createRoutes();
        loadCoordinates();
    }

    ~Graph()
    {
        for (int i = 0; i < ports.getSize(); i++)
        {
            delete ports[i];
        }
    }
};