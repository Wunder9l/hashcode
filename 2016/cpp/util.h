#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;
#define sqr(x) (x) * (x)

typedef uint64_t ui64;

template <typename T>
class ConstantKeeper {
public:
    T get(const string& key) {
        return Storage[key];
    }

    void registerConst(const string& key, T value) {
        Storage.insert(make_pair(key, value));
    }

private:
    unordered_map<string, T> Storage;
};

int distance(pair<int, int> p1, pair<int, int> p2);