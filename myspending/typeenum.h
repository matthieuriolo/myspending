#ifndef TYPEENUM_H
#define TYPEENUM_H
#include "typescheduler.h"
#include <map>

using namespace std;

const int TypeSchedulerNone = 0;
const int TypeSchedulerDaily = 1;
const int TypeSchedulerWeekly = 2;
const int TypeSchedulerMonthly = 3;
const int TypeSchedulerYearly = 4;

const map<int, TypeScheduler> TypeSchedulers = {
    {TypeSchedulerNone, TypeScheduler("none", 0)},
    {TypeSchedulerDaily, TypeScheduler("daily", 1)},
    {TypeSchedulerWeekly, TypeScheduler("weekly", 7)},
    {TypeSchedulerMonthly, TypeScheduler("monthly", 30)},
    {TypeSchedulerYearly, TypeScheduler("yearly", 360)},
};

#endif // TYPEENUM_H
