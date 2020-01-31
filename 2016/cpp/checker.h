#pragma once

#include "models.h"
#include "util.h"

class Checker {
public :
    Checker(const World& world, const vector<Product>& products, vector<Warehouse>  warehouses,
            vector<Order>  orders, vector<Drone>  drones);

    int CheckSolution(const vector<Command>& commands);

private:
    int PerformLoadCommand(const Command& c);

    int PerformDeliverCommand(const Command& c);

    const World& World;
    const vector<Product>& Products;
    vector<Warehouse> Warehouses;
    vector<Order> Orders;
    vector<Drone> Drones;
};


