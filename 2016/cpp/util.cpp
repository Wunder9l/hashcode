#include "util.h"
#include <cmath>
#include <sstream>

int distance(pair<int, int> p1, pair<int, int> p2) {
    return (int) std::ceil(std::sqrt(sqr(p1.first - p2.first) + sqr(p1.second - p2.second)));
}

template<typename T>
static string ParseConstant(const string& s, T& value) {
    if (auto pos = s.find('='); pos != 0 and pos != string::npos) {
        istringstream(s.substr(pos + 1)) >> value;
        return s.substr(0, pos);
    }
    assert(("Can not parse constants", false));
}

void TConstantStorage::ParseConstants(char **argv, std::size_t cnt) {
    const static string INT_CONST = "intConst";
    const static string DOUBLE_CONST = "doubleConst";
    for (std::size_t i = 0; i + 1 < cnt; ++i) {
        if (INT_CONST.compare(argv[i]) == 0) {
            int value;
            IntConstants.RegisterConst(ParseConstant(argv[i + 1], value), value);
        } else if (DOUBLE_CONST.compare(argv[i]) == 0) {
            double value;
            DoubleConstants.RegisterConst(ParseConstant(argv[i + 1], value), value);
        } else {
            assert(("Unknown parameter while parsing constants", true));
        }
    }
}

int TConstantStorage::GetInt(const string& key) const {
    return IntConstants.Get(key);
}

double TConstantStorage::GetDouble(const string& key) const {
    return DoubleConstants.Get(key);
}

void TConstantStorage::RegisterDouble(const string& key, double value) {
    DoubleConstants.RegisterConst(key, value);
}

void TConstantStorage::RegisterInt(const string& key, int value) {
    IntConstants.RegisterConst(key, value);
}
