#pragma once

#include "models.h"
#define ORDER_CMP "OrderCmp"

class Simulator {
public:
    Simulator(const World& world, vector <Product>  products, vector <Warehouse>  warehouses,
              vector <Order>  orders, vector <Drone>  drones, ConstantKeeper<int> intConst);
    vector<Command> Solve();
    void SortOrders();
    void SetOrders(vector<Order> orders);
    const vector<Order>& GetOrders();

private:
    bool OrderCmp(const Order& a, const Order& b);
    void MakeCommands(const vector<Command>& commands, Order& order);
    void TryCompleteOrder(Order& order);
    pair<int, vector<Command>> GetBestGoods(const Warehouse& warehouse, const Order& order);

    const World World;
    ConstantKeeper<int> IntConsts;
    vector <Product> Products;
    vector <Warehouse> Warehouses;
    vector <Order> Orders;
    vector <Drone> Drones;
    vector <Command> Commands;
};



