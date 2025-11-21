#pragma once
#include <iostream>
#include "Route.h"
#include "LinkedList.h"
struct Port
{
    string name;
    double portCharges;
    LinkedList<Route> routes;
    Port(string name, double portCharges)
    {
        this->name = name;
        this->portCharges = portCharges;
    }
    
};