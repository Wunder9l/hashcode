#pragma once

#include <functional>
#include <random>
#include <math.h>


template<class TState>
class IAnnealingSimulator {
public:
    /// Callback to be used during annealing to decide switching to a new  state
    /// \param state
    /// \return energy level (less is better)
    virtual double EnergyCallback(const TState &state) = 0;

    virtual TState NextStateCallback(const TState &state) = 0;

    virtual double TemperatureCallback(double curTemperature, uint64_t iteration) = 0;
};

template<class TState>
class Annealing {
public:
    using OnIterationCallback = std::function<void(uint64_t i, const TState& state, double energy, double temperature)>;

    explicit Annealing(IAnnealingSimulator<TState>& simulator) : Simulator(simulator) {
        std::random_device rd;
        Generator = std::mt19937(rd());
    }

    std::pair<TState, double> Run(double startTemperature, double endTemperature, const TState &startState,
                                  uint64_t maxIterations = 1000000, const OnIterationCallback *callback = nullptr) {
        double t = startTemperature;
        TState state = startState;
        for (uint64_t i = 0; i < maxIterations && t > endTemperature; ++i) {
            auto nextState = Simulator.NextStateCallback(state);
            double prevEnergy = Simulator.EnergyCallback(state);
            double nextEnergy = Simulator.EnergyCallback(nextState);
            if (DoStateChange(prevEnergy, nextEnergy, t)) {
                state = nextState;
                prevEnergy = nextEnergy;
            }
            t = Simulator.TemperatureCallback(t, i);
            if (callback) {
                (*callback)(i, state, prevEnergy, t);
            }
        }
        return std::make_pair(state, Simulator.EnergyCallback(state));
    }

private:
    bool DoStateChange(double prevEnergy, double nextEnergy, double temperature) {
        if (prevEnergy > nextEnergy) {
            return true;
        } else {
            double probability = exp((prevEnergy - nextEnergy) / temperature);
            return Distribution(Generator) <= probability;
        }
    }

    IAnnealingSimulator<TState>& Simulator;

    std::mt19937 Generator;
    std::uniform_real_distribution<double> Distribution = std::uniform_real_distribution<double> (0.0,1.0);
};

