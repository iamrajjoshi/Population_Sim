#ifndef Random_h
#define Random_h

#include "Common.h"

//Class random with functions defined in random.cpp
class Random {
private:
    double c;
    unsigned int dim;
    mt19937 mt_rand;
public:
    double getC();
    unsigned int getDim();
    double birthrate;
    double kidProbability();
    Random(double, unsigned int, double);
    vector <pair<double, double>> spawnImmigrants();
    pair <double, double> spawnFirstImmigrant();
    double generate(double, double);
};

#endif
