#include "simulator.h"

#include <utility>
#include <functional>

TSimulator::TSimulator(const struct TWorld& world, vector <TProduct>  products, vector <TWarehouse>  warehouses,
                       vector <TOrder>  orders, vector <TDrone>  drones, TConstantStorage consts)
    : World(world)
    , Products(std::move(products))
    , Warehouses(std::move(warehouses))
    , Orders(std::move(orders))
    , Drones(std::move(drones))
    , Constants(std::move(consts))
{
}

pair<int, vector<Command>> TSimulator::GetBestGoods(const TWarehouse& warehouse, const TOrder& order) {
    int weight = 0;
    int pWeight;
    vector<Command> commands;
    for (auto[id, cnt] : order.orderedProducts) {
        pWeight = Products[id].weight;
        int warehouseCnt = warehouse.productsCount[id];
        int productCnt = 0;
        while (cnt > productCnt && warehouseCnt > 0 && (World.maxWeight - weight >= (1 + productCnt) * pWeight)) {
            ++productCnt;
            --warehouseCnt;
        }
        if (productCnt) {
            cnt -= productCnt;
            weight += pWeight * productCnt;
            commands.push_back(
                    Command{.action = EAction::Load, .warehouse = warehouse.id, .product = id, .count = productCnt});
        }
    }
    auto loadProducts = commands.size();
    for (size_t i = 0; i < loadProducts; ++i) {
        commands.push_back(Command
                                   {.action = EAction::Deliver, .order = order.id, .product = commands[i].product, .count = commands[i].count});
    }
    return make_pair(weight, commands);
}

void TSimulator::MakeCommands(const vector<Command>& commands, TOrder& order) {
    pair<int, int> destination;
    for (const auto& c: commands) {
        if (c.action == EAction::Load) {
            Warehouses[c.warehouse].productsCount[c.product] -= c.count;
            Products[c.product].needed -= c.count;
            destination = Warehouses[c.warehouse].loc;
        } else if (c.action == EAction::Deliver) {
            assert(("Delivering product does not exist for order", order.orderedProducts.count(c.product)));
            if (order.orderedProducts[c.product] == c.count) {
                order.orderedProducts.erase(c.product);
            } else {
                order.orderedProducts[c.product] -= c.count;
            }
            order.totalWeight -= Products[c.product].weight * c.count;
            destination = order.loc;
        }
        Drones[c.drone].makeCommand(c, destination);
        Commands.push_back(c);
    }
}

void TSimulator::TryCompleteOrder(TOrder& order) {
    while (!order.isCompleted()) {
        int bestRes = 0;
        vector<Command> bestCommands;
        for (const auto& warehouse : Warehouses) {
            auto[res, commands] = GetBestGoods(warehouse, order);
            if (res > bestRes) {
                bestRes = res;
                bestCommands.swap(commands);
            }
        }
        assert(("Empty list of bestCommands", !bestCommands.empty()));
        int bestDrone = 0, bestCost = numeric_limits<int>::max(), cost;
        const auto& w = Warehouses[bestCommands[0].warehouse];
        for (const auto& drone : Drones) {
            cost = drone.timeToDone(w, order);
            if (cost < bestCost) {
                bestDrone = drone.id;
                bestCost = cost;
            }
        }
        for (auto& c : bestCommands) {
            c.drone = bestDrone;
        }

        MakeCommands(bestCommands, order);
    }
    order.complete();
}

vector<Command> TSimulator::Solve() {
    Commands.clear();
    for (auto& o : Orders) {
        TryCompleteOrder(o);
    }
    return Commands;
}

bool TSimulator::OrderCmp(const TOrder& a, const TOrder& b) {
    static const string key = ORDER_CMP;

    switch (Constants.GetInt(key)) {
        case 1:
            return a.totalWeight < b.totalWeight;
    }
    assert(("OrderCmp algorithm should be defined", false));
}

void TSimulator::SortOrders() {
    sort(Orders.begin(), Orders.end(), [this](const TOrder& a, const TOrder& b) { return this->OrderCmp(a, b); });
}

void TSimulator::SetOrders(vector<TOrder> orders) {
    Orders = std::move(orders);
}

void TSimulator::SetOrders(const vector<int>& ordersIds) {
    assert(("Orders must have same size", ordersIds.size() == Orders.size()));
    unordered_map<int, size_t> positions;
    for (size_t i = 0; i < Orders.size(); ++i){
        positions[Orders[i].id] = i;
        cout << "positions[" << i << "] = " << i << ", size=" << positions.size() << endl;
    }
    vector<TOrder> newOrders(Orders.size());
    for (size_t i = 0; i < ordersIds.size(); ++i){
        newOrders[i] = Orders[positions[ordersIds[i]]];
    }
    Orders.swap(newOrders);
}

const vector<TOrder>& TSimulator::GetOrders() {
    return Orders;
}
