#ifndef Common_h
#define Common_h

#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <random>
#include <chrono>
#include <functional>
#include <algorithm>
#include <numeric>
#include <thread>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <future>
#include <queue>

using namespace std;
using namespace std::chrono;

class Person {
public:
    unsigned int age;
    unsigned int fitness;
    Person(unsigned int age, unsigned int fitness) : age(age), fitness(fitness) {};
};

class City {
public:
    vector <Person> adults;
    vector <Person> kids;
    unsigned int totalPopulation;
    unsigned int totalKids;
    unsigned int totalAdults;
    double averageFitness;
    double averageAdultAge;
    double averageKidAge;
    double lifeexpectancy;
    double totalDeaths = 0;
    double cityAgeofDeath = 0;
    int AODcity = 0;
    City(double fitness, double age) : totalPopulation(1), totalKids(0), totalAdults(1), averageFitness(fitness), averageAdultAge(age), averageKidAge(0), totalDeaths(0), cityAgeofDeath(0), AODcity(0), lifeexpectancy(0) {};
    City(Person p) : totalPopulation(1), totalKids(0), totalAdults(1), averageFitness(p.fitness), averageAdultAge(p.age), averageKidAge(0), lifeexpectancy(0) {
        adults.push_back(p);
    };
};

#endif
