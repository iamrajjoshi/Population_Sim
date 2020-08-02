#ifndef World_h
#define World_h

#include "Common.h"
#include "Random.h"

class Person {
private:
    unsigned int age;
    unsigned int fitness;
    Person(unsigned int age, unsigned int fitness) : age(age), fitness(fitness) {};
    friend class City;
    friend class Simulation_Instance;
};

class City {
private:
    vector <Person> adults;
    vector <Person> kids;
    unsigned int totalPopulation;
    double averageFitness;
    double lifeExpectancy;
    City(Person p) : totalPopulation(1), averageFitness(p.fitness), lifeExpectancy(0) { adults.push_back(p); };
    friend class Simulation_Instance;
};

class Simulation_Instance {
private:
    ofstream outFile;
    string outFileName;
    
    Random generator;
    
    vector <string> data;
    vector <City> cities;
    vector <pair<double, double>> cityCenters;
    
    double alpha, beta, c0, moving, averageFitness, avgAgeOfDeath, numInfluencesCalculations, avgDistBtwn2Points;
    unsigned int date, totalPopulation, numberOfDeaths;

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
    Simulation_Instance(double,double, double, double, unsigned int, unsigned int, double, string);
    void simulate(unsigned int);
};

#endif
