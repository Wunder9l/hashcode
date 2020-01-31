#pragma once

#include "util.h"
#include <iostream>

enum class EAction {
    Load,
    Deliver
};

struct Command {
    EAction action;
    int drone;
    int warehouse;
    int order;
    int product;
    int count;

    void writeTo(std::ostream& out) const {
        out << drone << ' ';
        if (action == EAction::Load) {
            out << "L " << warehouse << ' ';
        } else {
            out << "D " << order << ' ';
        }
        out << product << ' ';
        out << count << endl;
    }

    void restoreFrom(std::istream& in) {
        in >> drone;
        char actionLetter;
        in >> actionLetter;
        if (actionLetter == 'L') {
            action = EAction::Load;
            in >> warehouse;
        } else if (actionLetter == 'D') {
            action = EAction::Deliver;
            in >> order;
        } else {
            assert(("Unsupported command action on read", false));
        }
        in >> product;
        in >> count;
    }
};

struct World {
    int height, width;
    int dronesCount;
    int maxTime;
    int maxWeight;

    int productsCount;
    int warehousesCount;
    int ordersCount;
};

struct Product {
    int id;
    int weight;
    int needed = 0;
};

struct Order {
    int id;
    pair<int, int> loc;
    unordered_map<int, int> orderedProducts;
    vector<int> wDistance;  // distance to each warehouse
    int totalWeight = 0;

    int lastTimestamp = 0;  // for accounting

//    void sumWeights() {
//        totalWeight = 0;
//        for (auto[id, cnt] : orderedProducts) {
//            totalWeight += products[id].weight * cnt;
//        }
//    }

    void complete() {
        totalWeight = 0;
        orderedProducts.clear();
    }

    bool isCompleted() const noexcept {
        return orderedProducts.empty();
    }

};


struct Warehouse {
    int id;
    pair<int, int> loc;
    vector<int> productsCount;


};

struct Drone {
    int id;
    pair<int, int> loc;
    int timestamp = 0;
    int product, productCnt;

    int curWeight = 0; // debug field

    Drone(const Warehouse& w) {
        product = productCnt = 0;
        this->loc = w.loc;
    }

    int timeToDone(const Warehouse& w, const Order& o) const {
        return timestamp + distance(loc, w.loc) + o.wDistance[w.id];
    }

    void makeCommand(const Command& c, pair<int, int> destination) {
        // only load and deliver
        timestamp += (destination == loc) ? 1 : 1 + distance(destination, loc);
        loc = destination;
    }
};
