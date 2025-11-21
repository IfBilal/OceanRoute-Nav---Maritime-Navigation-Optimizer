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
};