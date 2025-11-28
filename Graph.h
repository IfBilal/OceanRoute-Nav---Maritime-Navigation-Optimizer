#pragma once
#include <iostream>
#include "Port.h"
#include "Route.h"
#include "Vector.h"
#include "DateTime.h"
#include "LinkedList.h"
#include "MinHeap.h"
#include "Ship.h"
#include <fstream>
using namespace std;
const int DockingTime = 2; // in hours
struct Graph
{
    ::vector<Port *> ports;

    bool isNextDay(int depHour, int depMinute, int arrHour, int arrMinute)
    {
        return arrHour < depHour || (arrHour == depHour && arrMinute < depMinute);
    }

    int findPortIndex(const string &portName)
    {
        for (int i = 0; i < ports.getSize(); i++)
        {
            if (ports[i]->name == portName)
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
        string line;
        ::vector<MinHeap<Ship>> heaps(ports.getSize());
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
            DateTime arrivalTime(nextYear ? year + 1 : year, nextMonth ? month + 1 > 12 ? 1 : month + 1 : month, nextDay ? day + 1 > 31 ? 1 : day + 1 : day, arrivalHour, arrivalMinute);

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
        string line;
        while (getline(file, line))
        {
            string name = line.substr(0, line.find(" "));
            double portCharges = stod(line.substr(line.find(" ") + 1));
            Port *port = new Port(name, portCharges);
            ports.push_back(port);
        }
    }
    // void displayGraph()
    // {
    //     for (int i = 0; i < ports.getSize(); i++)
    //     {
    //         Node<Route> *current = ports[i]->routes.head;
    //         cout<<"\n===============================================================\n";
    //         cout << "Port: " << ports[i]->name << ", Charges: " << ports[i]->portCharges << endl;
    //         while (current != nullptr)
    //         {
    //             current->data.displayRoute();
    //             cout<< "Destination port name : " << ports[current->data.destinationIndex]->name << endl<<endl;
    //             current = current->next;
    //         }
    //     }
    // }
    Graph()
    {
        createPorts();
        createRoutes();
    }
};