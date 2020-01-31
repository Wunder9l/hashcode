#pragma once

#include "models.h"
#include "checker.h"
#include "simulator.h"
#include "annealing_simulator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

using namespace std;

World world;
vector<Product> products;
vector<Order> orders;
vector<Warehouse> warehouses;
vector<Drone> drones;
ConstantKeeper<int> intConsts;


void readInput(istream& input) {
    input >> world.height >> world.width >> world.dronesCount >> world.maxTime >> world.maxWeight;

    input >> world.productsCount;
    products.resize(world.productsCount);
    for (int i = 0; i < world.productsCount; i++) {
        input >> products[i].weight;
        products[i].id = i;
    }

    input >> world.warehousesCount;
    warehouses.resize(world.warehousesCount);
    for (int i = 0; i < world.warehousesCount; i++) {
        input >> warehouses[i].loc.first >> warehouses[i].loc.second;
        warehouses[i].id = i;
        warehouses[i].productsCount.resize(world.productsCount);
        for (int j = 0; j < world.productsCount; j++)
            input >> warehouses[i].productsCount[j];
    }

    input >> world.ordersCount;
    orders.resize(world.ordersCount);
    int productId, productInOrder;
    for (int i = 0; i < world.ordersCount; i++) {
        orders[i].id = i;
        input >> orders[i].loc.first >> orders[i].loc.second;
        input >> productInOrder;
        for (int j = 0; j < productInOrder; j++) {
            input >> productId;
            orders[i].orderedProducts[productId]++;
            products[productId].needed++;
            orders[i].totalWeight += products[productId].weight;
        }

        orders[i].wDistance.resize(world.warehousesCount);
        for (const auto& warehouse : warehouses) {
            orders[i].wDistance[warehouse.id] = distance(orders[i].loc, warehouse.loc);
        }
    }

    drones = vector<Drone>(world.dronesCount, Drone(warehouses[0]));
    for (int i = 0; i < drones.size(); ++i) {
        drones[i].id = i;
    }
}


void readFromFile(const char* filename) {
    filebuf fb;
    if (fb.open(filename, std::ifstream::in)) {
        istream input(&fb);
        readInput(input);
        fb.close();
    } else {
        assert(("Can not open input file", false));
    }
}

void writeToFile(const char* filename, vector<Command> commands) {
    filebuf fb;
    if (fb.open(filename, std::ifstream::out)) {
        ostream out(&fb);
        out << commands.size() << endl;
        for (const auto& c : commands) {
            c.writeTo(out);
        }
        fb.close();
    } else {
        assert(("Can not open output file", false));
    }
}

vector<Command> readCommands(const char* filename) {
    filebuf fb;
    vector<Command> commands;
    if (fb.open(filename, std::ifstream::in)) {
        istream input(&fb);
        int commandsNumber;
        input >> commandsNumber;
        commands.resize(commandsNumber);
        for (int i = 0; i < commandsNumber; ++i) {
            commands[i].restoreFrom(input);
        }
        fb.close();
    } else {
        assert(("Can not open file with commands", false));
    }
    return commands;
}

void DoAnnealing(string debugFilename) {
    Simulator simulator(world, products, warehouses, orders, drones, intConsts);
    Checker checker(world, products, warehouses, orders, drones);
    double startTemp = 64.0;
    ui64 maxIterations = 100000;
    DeliverySimulator deliverySimulator(startTemp, simulator, checker);

//    simulator.SortOrders();
    DeliverySimulatorState startState{.Orders = simulator.GetOrders()};

    auto annealing = Annealing<DeliverySimulatorState>(deliverySimulator);
    DebugClass debug(maxIterations);
    std::function<void(uint64_t, const DeliverySimulatorState&, double, double)> callback =
            [&debug](uint64_t i, const DeliverySimulatorState& state, double energy, double temperature) {
                debug.AddLogEntry(i, state, energy, temperature);
            };
    const auto&[state, res] = annealing.Run(startTemp, 0.0001, startState, maxIterations, &callback);
    cout << "Result of annealing simulation: " << deliverySimulator.GetPoints(state) << ", energy=" << res << endl;
    for (const auto& o : state.Orders) {
        cout << ' ' << o.id;
    }
    debug.WriteToFile(debugFilename);
}

int main(int argc, char* argv[]) {
    readFromFile(argv[2]);  // save to global structures
    intConsts.registerConst(ORDER_CMP, 1);  // atoi(argv[4]))

    vector<Command> commands;
    if (!strcmp(argv[1], "solve")) {
        Simulator simulator(world, products, warehouses, orders, drones, intConsts);
        simulator.SortOrders();
        commands = simulator.Solve();
        writeToFile(argv[3], commands);
    } else if (!strcmp(argv[1], "eval")) {
        commands = readCommands(argv[3]);
    } else if (!strcmp(argv[1], "annealing")) {
        DoAnnealing(argv[3]);
    }
    Checker checker(world, products, warehouses, orders, drones);
    int res = checker.CheckSolution(commands);
    cout << "Result of " << argv[1] << " (" << argv[2] << ", " << argv[3] << ") is " << res << endl;

}