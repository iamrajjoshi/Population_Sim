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
    unsigned int totalAdults;
    double averageFitness;
    double averageAdultAge;
    double lifeExpectancy;
    City(double fitness, double age) : totalPopulation(1), totalAdults(1), averageFitness(fitness), averageAdultAge(age), lifeExpectancy(0) {};
    City(Person p) : totalPopulation(1), totalAdults(1), averageFitness(p.fitness), averageAdultAge(p.age), lifeExpectancy(0) {
        adults.push_back(p);
    };
};

#endif
