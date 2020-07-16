#ifndef World_h
#define World_h

#include "Common.h"
#include "Immigrant_Generator.h"

class World {
    friend class Excel_Helper;
private:
    ofstream outFile;
    string outFileName;
    vector <string> data;
    vector <string> imiplace;
    vector <string> movplace;
    Immigrant_Generator generator;
    vector <City> cities;
    vector <pair<double, double>> cityCenters;
    //inputs
    double a;
    double b;
    double c0;
    double m;
    unsigned int date;
    unsigned int totalCities;//after moving
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
    double calculateInfluence(unsigned int, pair<double, double>point);
public:
    int ac,caod,totalkids;
    double aod;
    double r[2]={0,0};
    World(double,double, double, double, unsigned int, unsigned int, double, string);//alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    void initializeSimulation();//TODO
    void advanceSimulation(unsigned int);
    void printData();//TODO
    void writeData();//TODO
};

#endif
