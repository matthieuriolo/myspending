#ifndef TYPEENUM_H
#define TYPEENUM_H
#include <map>
#include <string>

using namespace std;

const map<int, string> TypeSchedule = {
    {0, "none"},
    {1, "daily"},
    {2, "weekly"},
    {3, "monthly"},
    {4, "yearly"},
};

#endif // TYPEENUM_H
