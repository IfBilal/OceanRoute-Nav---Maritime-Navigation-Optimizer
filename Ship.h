#pragma once
#include "DateTime.h"
#include <iostream>
using namespace std;
struct Ship
{
    DateTime arrivalTime;
    string companyName;
    string destinationPort;
    string sourcePort;

    Ship(DateTime arrivalTime = DateTime(), string companyName = "", string destinationPort = "", string sourcePort = "")
    {
        this->arrivalTime = arrivalTime;
        this->companyName = companyName;
        this->destinationPort = destinationPort;
        this->sourcePort = sourcePort;
    }
    bool operator<(const Ship &other) const
    {
        return arrivalTime < other.arrivalTime;
    }
    bool operator>(const Ship &other) const
    {
        return arrivalTime > other.arrivalTime;
    }
    bool operator==(const Ship &other) const
    {
        return arrivalTime == other.arrivalTime && companyName == other.companyName && destinationPort == other.destinationPort && sourcePort == other.sourcePort;
    }
    bool operator!=(const Ship &other) const
    {
        return !(*this == other);
    }
};