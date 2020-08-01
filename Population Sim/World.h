#ifndef World_h
#define World_h

#include "Common.h"
#include "Immigrant_Generator.h"

class World {
private:
    ofstream outFile;
    string outFileName;
    
    Immigrant_Generator generator;
    
    vector <string> data;
    vector <City> cities;
    vector <pair<double, double>> cityCenters;
    
    double alpha, beta, c0, moving, averageFitness, avgAgeOfDeath, numInfluencesCalculations;
    unsigned int date, totalPopulation, numberOfDeaths;
    double avgDistBtwn2Points;
    
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
   
public:
     //alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    World(double,double, double, double, unsigned int, unsigned int, double, string);
    void simulate(unsigned int);
};

#endif
