#ifndef TYPESCHEDULER_H
#define TYPESCHEDULER_H

#include <string>

using namespace std;

class TypeScheduler {
private:
    string name;
    double rate;
public:
    TypeScheduler(string name, double rate);

    string getName() const;
    double convertToSmallestUnit(double value) const;
    double convertToSameUnit(double value) const;
};


#endif // TYPESCHEDULER_H
