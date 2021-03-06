#include "models.h"
#include "checker.h"
#include "simulator.h"
#include "annealing_simulator.h"
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

TWorld world;
vector<TProduct> products;
vector<TOrder> orders;
vector<TWarehouse> warehouses;
vector<TDrone> drones;
TConstantStorage constants;

#define INT_CONST_ITERATIONS "iterations"
#define INT_CONST_START_T "startT"
#define INT_CONST_END_T "endT"

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

    drones = vector<TDrone>(world.dronesCount, TDrone(warehouses[0]));
    for (int i = 0; i < drones.size(); ++i) {
        drones[i].id = i;
    }
}


void readFromFile(const char *filename) {
    filebuf fb;
    if (fb.open(filename, std::ifstream::in)) {
        istream input(&fb);
        readInput(input);
        fb.close();
    } else {
        assert(("Can not open input file", false));
    }
}

void writeToFile(const char *filename, vector<Command> commands) {
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

vector<Command> readCommands(const char *filename) {
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

vector<int> ReadOrders(const string& filename) {
    filebuf fb;
    vector<int> orderIds;
    if (fb.open(filename, std::ifstream::in)) {
        istream input(&fb);
        orderIds.resize(orders.size());
        ui64 iteration;
        double energy, temp;
        input >> iteration >> energy >> temp;
        for (int i = 0; i < orders.size(); ++i) {
            input >> orderIds[i];
        }
        fb.close();
    } else {
        assert(("Can not open file with dumpState", false));
    }
    return orderIds;
}

void DoAnnealing(const string& debugFilename, const string& dumpFilename) {
    TSimulator simulator(world, products, warehouses, orders, drones, constants);
    Checker checker(world, products, warehouses, orders, drones);
    const double startTemp = constants.GetDouble(INT_CONST_START_T);
    const double endTemp = constants.GetDouble(INT_CONST_END_T);
    DeliverySimulator deliverySimulator(startTemp, simulator, checker);

    if (!dumpFilename.empty() && dumpFilename != "-") {
        simulator.SetOrders(ReadOrders(dumpFilename));
    }
//    simulator.SortOrders();
    DeliverySimulatorState startState{.Orders = simulator.GetOrders()};

    auto annealing = Annealing<DeliverySimulatorState>(deliverySimulator);
    const ui64 maxIterations = constants.GetInt(INT_CONST_ITERATIONS);
    DebugClass debug(maxIterations);
    std::function<void(uint64_t, const DeliverySimulatorState&, double, double)> callback =
            [&debug](uint64_t i, const DeliverySimulatorState& state, double energy, double temperature) {
                debug.AddLogEntry(i, state, energy, temperature);
            };
    cout << "Start of annealing simulation: T_start=" << startTemp << ", T_end=" << endTemp << ", maxIterations=" << maxIterations << endl;
    cout << "Start state (energy=" << deliverySimulator.EnergyCallback(startState) << "):" << endl;
    for (auto& o: startState.Orders) {
        cout << ' ' << o.id;
    }
    cout << std::endl;
    const auto&[state, res] = annealing.Run(startTemp, endTemp, startState, maxIterations, &callback);
    cout << "Result of annealing simulation: " << deliverySimulator.GetPoints(state) << ", energy=" << res << endl;
    for (const auto& o : state.Orders) {
        cout << ' ' << o.id;
    }
    debug.WriteToFile(debugFilename);
}

int main(int argc, char *argv[]) {
    readFromFile(argv[2]);  // save to global structures
    constants.RegisterInt(ORDER_CMP, 1);  // atoi(argv[4]))

    vector<Command> commands;
    if (!strcmp(argv[1], "solve")) {
        TSimulator simulator(world, products, warehouses, orders, drones, constants);
        simulator.SortOrders();
        commands = simulator.Solve();
        writeToFile(argv[3], commands);
    } else if (!strcmp(argv[1], "eval")) {
        commands = readCommands(argv[3]);
    } else if (!strcmp(argv[1], "annealing")) {
        assert(("Too few arguments, {exe-file} annealing {input filename} {output filename} "
                "[constants {intConst name=value doubleConst name=value}]", argc >= 4));
        constants.ParseConstants(argv + 5, argc - 5);
        DoAnnealing(argv[3], argv[4]);
    }
    Checker checker(world, products, warehouses, orders, drones);
    int res = checker.CheckSolution(commands);
    cout << "Result of " << argv[1] << " (" << argv[2] << ", " << argv[3] << ") is " << res << endl;

}