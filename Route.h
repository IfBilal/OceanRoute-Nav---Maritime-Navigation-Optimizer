#pragma once

#include <iostream>
#include "Port.h"
#include "DateTime.h"
using namespace std;

struct Route
{
    Port *destinationPort;
    DateTime departureTime;
    DateTime arrivalTime;
    double voyageCost;
    string company;
    Route *next;

    Route(Port *destinationPort, DateTime departureTime, DateTime arrivalTime, double voyageCost, string company)
    {
        this->destinationPort = destinationPort;
        this->departureTime = departureTime;
        this->arrivalTime = arrivalTime;
        this->voyageCost = voyageCost;
        this->company = company;
        next = nullptr;
    }
};