#pragma once
#include <iostream>
#include "Route.h"
#include "LinkedList.h"
#include "Queue.h"
#include "Ship.h"

struct Port
{
    string name;
    double portCharges;
    LinkedList<Route> routes;
    Queue<Ship> shipsQueue;
    double latitude;
    double longitude;
    
    Port(string name, double portCharges)
    {
        this->name = name;
        this->portCharges = portCharges;
        this->latitude = 0.0;
        this->longitude = 0.0;
    }
    
};