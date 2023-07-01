#ifndef TYPEENUM_H
#define TYPEENUM_H
#include "typescheduler.h"
#include <map>

using namespace std;

const map<int, TypeScheduler> TypeSchedulers = {
    {0, TypeScheduler("none", 0)},
    {1, TypeScheduler("daily", 1)},
    {2, TypeScheduler("weekly", 7)},
    {3, TypeScheduler("monthly", 30)},
    {4, TypeScheduler("yearly", 360)},
};

#endif // TYPEENUM_H
