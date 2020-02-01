#pragma once

#include "models.h"
#include "util.h"

class Checker {
public :
    Checker(const TWorld& world, const vector<TProduct>& products, vector<TWarehouse>  warehouses,
            vector<TOrder>  orders, vector<TDrone>  drones);

    int CheckSolution(const vector<Command>& commands);

private:
    int PerformLoadCommand(const Command& c);

    int PerformDeliverCommand(const Command& c);

    const TWorld& World;
    const vector<TProduct>& Products;
    vector<TWarehouse> Warehouses;
    vector<TOrder> Orders;
    vector<TDrone> Drones;
};


