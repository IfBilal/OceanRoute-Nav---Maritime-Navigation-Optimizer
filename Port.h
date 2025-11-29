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
    Port(string name, double portCharges)
    {
        this->name = name;
        this->portCharges = portCharges;
    }
    
};