#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <unordered_map>
#include <cassert>

using namespace std;
#define sqr(x) (x) * (x)

typedef uint64_t ui64;

template<typename T>
class TConstantKeeper {
public:
    T Get(const string& key) const {
        if (auto it = Storage.find(key); it != Storage.end()) {
            return it->second;
        }
        assert(("Can not get constant with such name", false));
    }

    void RegisterConst(const string& key, T value) {
        Storage.insert(make_pair(key, value));
    }

private:
    unordered_map<string, T> Storage;
};

class TConstantStorage {
public:
    void ParseConstants(char *argv[], std::size_t cnt);
    int GetInt(const string& key) const;
    void RegisterInt(const string& key, int value);
    double GetDouble(const string& key) const;
    void RegisterDouble(const string& key, double value);

private:
    TConstantKeeper<int> IntConstants;
    TConstantKeeper<double> DoubleConstants;
};

int distance(pair<int, int> p1, pair<int, int> p2);