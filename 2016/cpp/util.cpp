#include "util.h"

int distance(pair<int, int> p1, pair<int, int> p2) {
    return (int) ceil(sqrt(sqr(p1.first - p2.first) + sqr(p1.second - p2.second)));
}