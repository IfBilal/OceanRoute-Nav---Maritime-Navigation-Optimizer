#pragma once
#include <iostream>
#include "Port.h"
#include "Route.h"
#include "Vector.h"
#include "DateTime.h"
#include <fstream>
using namespace std;
struct Graph
{
    vector<Port *> ports;

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
    void createRoutes()
    {
        ifstream file("Routes.txt");
        string line;
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

            DateTime departureTime(year, month, day, departureHour, departureMinute);
            DateTime arrivalTime(year, month, (nextDay) ? day + 1 : day, arrivalHour, arrivalMinute);

            int sourceIndex = -1;
            int destinationIndex = -1;
            sourceIndex = findPortIndex(sourcePortName);
            destinationIndex = findPortIndex(destinationPortName);
            if (sourceIndex != -1 && destinationIndex != -1)
            {
                Route route(destinationIndex, departureTime, arrivalTime, voyageCost, company);
                ports[sourceIndex]->routes.insertAtHead(route);
            }
            else
            {
                cout << "Error: Invalid port name in route data." << endl;
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
    Graph()
    {
        createPorts();
        createRoutes();
    }
};