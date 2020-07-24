#ifndef World_h
#define World_h

#include "Common.h"
#include "Immigrant_Generator.h"

class World {
private:
    ofstream outFile;
    string outFileName;
    vector <string> data;
    Immigrant_Generator generator;
    vector <City> cities;
    vector <pair<double, double>> cityCenters;
    double alpha;
    double beta;
    double c0;
    double moving;
    unsigned int date;
    unsigned int totalCities;
    unsigned int totalPopulation;
    double averageFitness;
    void writeFile();
    void addData();
    void advanceAge();
    void advanceImmigrantMechanic();
    void advanceFitness();
    unsigned int calculateFitness(unsigned int);
    void advanceMovingMechanic();
    unsigned int calculateLifeExpectancy(unsigned int);
    long double calculateInfluence(unsigned int, pair<double, double>point);
    void initializeSimulation();
    unsigned int ac,caod,totalkids;
    double aod, r[2]={0,0};
public:
     //alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    World(double,double, double, double, unsigned int, unsigned int, double, string);
    void simulate(unsigned int);
};

#endif
