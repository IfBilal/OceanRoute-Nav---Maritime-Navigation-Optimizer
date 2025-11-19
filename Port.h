#pragma once
#include <iostream>
#include "Route.h"
struct Port
{
    string name;
    double portCharges;
    Route *routes;
    Port(string name, double portCharges)
    {
        this->name = name;
        this->portCharges = portCharges;
        routes = nullptr;
    }
};