#include "simulator.h"

#include <utility>
#include <functional>

Simulator::Simulator(const struct World& world, vector <Product>  products, vector <Warehouse>  warehouses,
                     vector <Order>  orders, vector <Drone>  drones, ConstantKeeper<int> intConst)
    : World(world)
    , Products(std::move(products))
    , Warehouses(std::move(warehouses))
    , Orders(std::move(orders))
    , Drones(std::move(drones))
    , IntConsts(std::move(intConst))
{
}

pair<int, vector<Command>> Simulator::GetBestGoods(const Warehouse& warehouse, const Order& order) {
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

void Simulator::MakeCommands(const vector<Command>& commands, Order& order) {
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

void Simulator::TryCompleteOrder(Order& order) {
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

vector<Command> Simulator::Solve() {
    Commands.clear();
    for (auto& o : Orders) {
        TryCompleteOrder(o);
    }
    return Commands;
}

bool Simulator::OrderCmp(const Order& a, const Order& b) {
    static const string key = ORDER_CMP;

    switch (IntConsts.get(key)) {
        case 1:
            return a.totalWeight < b.totalWeight;
    }
    assert(("OrderCmp algorithm should be defined", false));
}

void Simulator::SortOrders() {
    sort(Orders.begin(), Orders.end(), [this](const Order& a, const Order& b) { return this->OrderCmp(a, b); });
}

void Simulator::SetOrders(vector<Order> orders) {
    Orders = std::move(orders);
}

const vector<Order>& Simulator::GetOrders() {
    return Orders;
}
