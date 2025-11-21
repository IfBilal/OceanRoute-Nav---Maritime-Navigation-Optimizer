#pragma once

#include <iostream>
#include "Port.h"
#include "DateTime.h"
using namespace std;

struct Route
{
    int destinationIndex;
    DateTime departureTime;
    DateTime arrivalTime;
    double voyageCost;
    string company;

    Route(int destinationIndex, DateTime departureTime, DateTime arrivalTime, double voyageCost, string company)
    {
        this->destinationIndex = destinationIndex;
        this->departureTime = departureTime;
        this->arrivalTime = arrivalTime;
        this->voyageCost = voyageCost;
        this->company = company;
    }
    void displayRoute()
    {
        cout << "Destination Index: " << destinationIndex << endl;
        cout << "Departure Time: " << departureTime.hour << ":" << departureTime.minute << " " << departureTime.day << "/" << departureTime.month << "/" << departureTime.year << endl;
        cout << "Arrival Time: " << arrivalTime.hour << ":" << arrivalTime.minute << " " << arrivalTime.day << "/" << arrivalTime.month << "/" << arrivalTime.year << endl;
        cout << "Voyage Cost: " << voyageCost << endl;
        cout << "Company: " << company << endl;
    }
};