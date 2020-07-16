#ifndef Immigrant_Generator_h
#define Immigrant_Generator_h
#include "Common.h"

class Immigrant_Generator {
private:
    double c;
    unsigned int dim;
    mt19937 mt_rand;
public:
    double birthrate;
    double kidProbability();
    Immigrant_Generator(double, unsigned int, double);
    vector <pair<double, double>> spawnImmigrants();
    pair <double, double> spawnFirstImmigrant();
    double rng(double, double);
};

#endif /* Immigrant_Generator_h */
