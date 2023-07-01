#include "typescheduler.h"

TypeScheduler::TypeScheduler(string name, double rate) : name(name), rate(rate) {}

string TypeScheduler::getName() const {
    return name;
}

double TypeScheduler::convertToSmallestUnit(double value) const {
    if (rate == 0.0) {
        return 0.0;
    }

    return value / rate;
}

double TypeScheduler::convertToSameUnit(double value) const {
    return value * rate;
}
