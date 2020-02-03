#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <random>
#include <fstream>
#include "annealing.h"
#include "simulator.h"
#include "checker.h"


class DeliverySimulatorState {
public:
    std::vector<TOrder> Orders;
};

class DeliverySimulator : public IAnnealingSimulator<DeliverySimulatorState> {
    double StartTemperature;
    TSimulator EnvSimulator;
    Checker SolutionChecker;
public:
    DeliverySimulator(double startTemperature, TSimulator simulator, Checker checker)
            : StartTemperature(startTemperature), EnvSimulator(std::move(simulator)),
              SolutionChecker(std::move(checker)) {
        EnvSimulator.SortOrders();
    }

    int GetPoints(const DeliverySimulatorState& state) {
        auto tmpSimulator = EnvSimulator;
        auto tmpChecker = SolutionChecker;
        tmpSimulator.SetOrders(state.Orders);
        return tmpChecker.CheckSolution(tmpSimulator.Solve());
    }

    double EnergyCallback(const DeliverySimulatorState& state) override {
        const static double MULTIPLIER = 100000.0;
        int val = EnvSimulator.Constants.GetInt("energy");
        if (val == 1) {
            return MULTIPLIER / (double) GetPoints(state);
        }
        return -GetPoints(state);
    }

    DeliverySimulatorState NextStateCallback(const DeliverySimulatorState& state) override {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, state.Orders.size() - 1);
        auto left = dis(gen), right = dis(gen);
        if (right < left) {
            std::tie(left, right) = std::tie(right, left);
        } else if (right == left) {
            if (left != 0) {
                left--;
            } else {
                right++;
            }
        }
        DeliverySimulatorState nextState = state;
        while (left < right) {
            std::swap(nextState.Orders[left], nextState.Orders[right]);
            ++left;
            --right;
        }
        return nextState;
    }

    double TemperatureCallback(double curTemperature, uint64_t iteration) override {
        return StartTemperature / iteration;
    }
};

class DebugClass {
    std::ostringstream output;
    const uint64_t MaxIterations;
public:
    DebugClass(uint64_t maxIterations) : MaxIterations(maxIterations) {}

    void AddLogEntry(uint64_t i, const DeliverySimulatorState& state, double energy, double temperature) {
        output << i << ' ' << energy << ' ' << temperature;
        for (auto& o: state.Orders) {
            output << ' ' << o.id;
        }
        output << std::endl;
        if (0 == i % (MaxIterations / 10)) {
            cout << i << ": energy=" << energy << ", t=" << temperature << endl;
        }
    }

    void WriteToFile(const std::string& filename) {
        filebuf fb;
        if (fb.open(filename, std::ifstream::out)) {
            ostream out(&fb);
            out << output.str();
            fb.close();
        } else {
            assert(("Can not open output file for DebugClass::WriteToFile", false));
        }
    }
};
