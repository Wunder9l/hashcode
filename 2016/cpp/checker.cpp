#include "checker.h"

#include <utility>


Checker::Checker(const struct World& world, const vector<Product>& products, vector<Warehouse>  warehouses,
                 vector<Order>  orders, vector<Drone>  drones)
     : World(world)
     , Products(products)
     , Warehouses(std::move(warehouses))
     , Orders(std::move(orders))
     , Drones(std::move(drones))
{
}


int Checker::CheckSolution(const vector<Command>& commands) {
    int points = 0;

    size_t i = 0;
    for (const auto& c : commands) {
        switch (c.action) {
            case EAction::Load:
                points += PerformLoadCommand(c);
                break;
            case EAction::Deliver:
                points += PerformDeliverCommand(c);
                break;
            default:
                assert(("Not supporting command type", false));
        }
        ++i;
    }
    return points;
}

int Checker::PerformLoadCommand(const Command& c) {
    assert(("Not enough items on warehouse", Warehouses[c.warehouse].productsCount[c.product] >= c.count));
    Warehouses[c.warehouse].productsCount[c.product] -= c.count;
    Drones[c.drone].makeCommand(c, Warehouses[c.warehouse].loc);
    Drones[c.drone].curWeight += c.count * Products[c.product].weight;
    assert(("More than max weight for drone", Drones[c.drone].curWeight <= World.maxWeight));
    return 0;
}

int Checker::PerformDeliverCommand(const Command& c) {
    assert(("More products than in order", Orders[c.order].orderedProducts[c.product] >= c.count));
    if (Orders[c.order].orderedProducts[c.product] == c.count) {
        Orders[c.order].orderedProducts.erase(c.product);
    } else {
        Orders[c.order].orderedProducts[c.product] -= c.count;
    }
    Drones[c.drone].makeCommand(c, Orders[c.order].loc);
    Drones[c.drone].curWeight -= c.count * Products[c.product].weight;
    assert(("Drone has negative weight", Drones[c.drone].curWeight >= 0));

    Orders[c.order].lastTimestamp = std::max(Orders[c.order].lastTimestamp, Drones[c.drone].timestamp);
    if (Orders[c.order].isCompleted()) {
        return std::ceil(100 * (double) (World.maxTime - Orders[c.order].lastTimestamp) / World.maxTime);
    }
    return 0;
}
