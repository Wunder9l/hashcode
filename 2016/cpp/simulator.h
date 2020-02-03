#pragma once

#include "models.h"
#define ORDER_CMP "OrderCmp"

class TSimulator {
public:
    TSimulator(const TWorld& world, vector <TProduct>  products, vector <TWarehouse>  warehouses,
               vector <TOrder>  orders, vector <TDrone>  drones, TConstantStorage consts);
    vector<Command> Solve();
    void SortOrders();
    void SetOrders(vector<TOrder> orders);
    const vector<TOrder>& GetOrders();

private:
    bool OrderCmp(const TOrder& a, const TOrder& b);
    void MakeCommands(const vector<Command>& commands, TOrder& order);
    void TryCompleteOrder(TOrder& order);
    pair<int, vector<Command>> GetBestGoods(const TWarehouse& warehouse, const TOrder& order);

    const TWorld World;
    vector <TProduct> Products;
    vector <TWarehouse> Warehouses;
    vector <TOrder> Orders;
    vector <TDrone> Drones;
    vector <Command> Commands;
public:
    const TConstantStorage Constants;
};



