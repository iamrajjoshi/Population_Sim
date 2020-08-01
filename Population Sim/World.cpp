#include "World.h"

World::World(double alpha, double beta, double birthrate, double constant_of_influence, unsigned int immigration_rate, unsigned int dim, double moving, string fileName) : generator(immigration_rate, dim, birthrate), alpha(alpha), beta(beta), c0(constant_of_influence), moving(moving), outFileName(fileName) {};

void World::initializeSimulation() {
    date = 0;
    avgAgeOfDeath = 0;
    numberOfDeaths = 0;
    averageFitness = 0;
    numInfluencesCalculations = 0;
    avgDistBtwn2Points = 0;
    pair<unsigned int, unsigned int> p(generator.spawnFirstImmigrant());
    cityCenters.push_back(p);
    Person person(18, calculateFitness(18));
    cities.push_back(person);
    totalPopulation = 1;
    averageFitness = person.fitness;
    calculateLifeExpectancy(0);
    return;
}

void World::advanceAge() {
    //increase age and transfer kids to adults
    for (int i = 0; i < cities.size(); ++i) {
        for (int j = 0; j < cities[i].adults.size(); ++j)//increment adult age
            cities[i].adults[j].age++;
        
        for (int j = 0; j < cities[i].kids.size(); ++j)//increment kids age
            cities[i].kids[j].age++;
        for (int j = 0; j < cities[i].kids.size(); ++j)//for each kid
            if (cities[i].kids[j].age == 18) {
                cities[i].adults.push_back(cities[i].kids[j]);//add to adult
                cities[i].kids.erase(cities[i].kids.begin() + j);//remove from kids vector
                j--;
            }
        
        for (int j = 0; j < cities[i].adults.size(); ++j) {//birth & death
            if (cities[i].adults[j].age == 30) {
                int it = generator.birthrate / 1;
                for (int uu = 0; uu < it; uu++)
                    cities[i].kids.push_back(Person(0, 0));
                if (generator.kidProbability() < generator.birthrate - it)
                    cities[i].kids.push_back(Person(0, 0));
            }
            if (cities[i].adults[j].age > calculateLifeExpectancy(i)) {
                avgAgeOfDeath = double((avgAgeOfDeath * numberOfDeaths + cities[i].adults[j].age)) / (numberOfDeaths + 1);
                numberOfDeaths++;
                cities[i].adults.erase(cities[i].adults.begin() + j);
                j--;
            }
        }
    }
    return;
}

unsigned int World::calculateLifeExpectancy(unsigned int index) {
    double y = (double(cities[index].averageFitness) / this->averageFitness) - 0.3;
    double output = roundl(114 * exp(y) / (exp(y) + 1));
    cities[index].lifeExpectancy = output;
    return output;
}

void World::advanceImmigrantMechanic() {
    //assigning immigrants to a city
    vector <pair<double, double>> immigrants(generator.spawnImmigrants());
    for (int i = 0; i < immigrants.size(); ++i) {
        
        vector<long double> influences;
        for (int j = 0; j < this->cities.size(); ++j)
            influences.push_back(calculateInfluence(j, immigrants[i]));
        
        long double total_influence = accumulate(influences.begin(), influences.end(), 0.0);
        total_influence += 1/total_influence;
        if (isnan(total_influence)) {
            cout << "ERROR: TOTAL INFLUENCE IS NAN" << endl;
            cout << "WRITING CURRENT DATA TO: " << this->outFileName << endl;
            writeFile();
            return;
        }
        
        vector<long double> probabilities(cities.size() + 1, 0);
        for(int j = 1; j < probabilities.size(); ++j)
            probabilities[j] = probabilities[j-1] + (influences[j-1])/total_influence;
        
        long double num = generator.rng(0, probabilities[probabilities.size()-1]+(long double)1/total_influence);
        
        if (num > probabilities[probabilities.size() - 1]) {
            cities.push_back(City(calculateFitness(18), 18));
            cityCenters.push_back(immigrants[i]);
            cities[cities.size() - 1].adults.emplace_back(Person(18, 31));
        }
        
        else
            for (int j = 1; j < probabilities.size(); ++j)
                if (probabilities[j - 1] < num && probabilities[j] >= num) {
                    cities[j - 1].adults.emplace_back(Person(18, 31));
                    break;
                }
    }
    
    return;
}

long double World::calculateInfluence(unsigned int index, pair<double, double> point) {
    long double d = sqrt(pow((this->cityCenters[index].first - point.first), 2) + pow((this->cityCenters[index].second - point.second), 2));
    avgDistBtwn2Points = (avgDistBtwn2Points * numInfluencesCalculations + d) / (numInfluencesCalculations + 1);
    numInfluencesCalculations++;
    long double influence = (c0 * pow(d, -beta)) * pow(cities[index].adults.size(), alpha);
    return influence;
}

unsigned int World::calculateFitness(unsigned int age) {
    double temp = age;
    double d = -((temp - 45) * (temp - 45)) / 625;//-(pow(age - 45,2)) / 625
    double output = 100 * exp(d);
    return floor(output);
}

void World::advanceFitness() {
    for (int i = 0; i < cities.size(); ++i) {
        for (int j = 0; j < cities[i].adults.size(); ++j)//increment adult fitness
            cities[i].adults[j].fitness = calculateFitness(cities[i].adults[j].age);
        for (int j = 0; j < cities[i].kids.size(); ++j)//increment kids fitness
            cities[i].kids[j].fitness = calculateFitness(cities[i].kids[j].age);
        
        cities[i].averageFitness = 0;
        for (int j = 0; j < cities[i].adults.size(); ++j)
            cities[i].averageFitness += cities[i].adults[j].fitness;
        if (cities[i].averageFitness == 0);
        else cities[i].averageFitness = cities[i].averageFitness / cities[i].adults.size();
    }
    
    this->averageFitness = 0;
    unsigned int activeCities = 0;
    for (int j = 0; j < cities.size(); ++j)
        if (cities[j].averageFitness) {
            averageFitness += cities[j].averageFitness;
            activeCities++;
        }
    averageFitness = averageFitness / activeCities;
    return;
}

void World::advanceMovingMechanic() {
    vector<pair<Person,unsigned int>> people;

    for (int i = 0; i < cities.size(); ++i) {
           for (int j = 0; j < cities[i].adults.size(); ++j) {
               if (cities[i].adults[j].fitness * (1 + (moving / 100)) > averageFitness) {
                   people.push_back(make_pair(cities[i].adults[j], i));
                   cities[i].adults.erase(cities[i].adults.begin() + j);
                   j--;
               }
               
           }
       }
    
    vector<double> probabilities(cities.size(), 0);
    double baseline = 0;
    
    for (int j = 0; j < cities.size(); ++j) {
        double temp = cities[j].averageFitness / averageFitness / cities.size();
        baseline += temp;
        probabilities[j] = baseline;
    }
    
    for (int i = 0; i < people.size(); ++i) {
        double num = generator.rng(0, baseline);
        for (int j = 0; j < probabilities.size(); ++j) {
            if (j == 0 && num < probabilities[j]) {
                if(people[i].second != j)
                    cities[j].adults.emplace_back(people[i].first);
                else {
                    unsigned x = people[i].second;
                    while(x != people[i].second)
                        x = int(generator.rng(0,cities.size()-1));
                    cities[x].adults.emplace_back(people[i].first);
                }
            }
                
            else if (probabilities[j - 1] < num && probabilities[j] >= num) {
                if(people[i].second != (j-1))
                    cities[j].adults.emplace_back(people[i].first);
                else {
                    unsigned x = people[i].second;
                    while(x != people[i].second)
                        x = int(generator.rng(0,cities.size()-1));
                    cities[x].adults.emplace_back(people[i].first);
                }
            }
        }
    }
    
    for (int i = 0; i < cities.size(); ++i)
        cities[i].totalPopulation = (unsigned int)(cities[i].adults.size() + cities[i].kids.size());
    
    this->totalPopulation = 0;
    for (int j = 0; j < cities.size(); ++j)
        totalPopulation += cities[j].totalPopulation;
    return;
}


void World::simulate(unsigned int count) {
    initializeSimulation();
    addData();
    for (int i = 0; i < count; ++i) {
        date++;
        advanceAge();
        advanceImmigrantMechanic();
        advanceFitness();
        advanceMovingMechanic();
        addData();
    }
    writeFile();
    return;
}

void World::addData() {
    string temp;
    temp += to_string(this->date) + ",";
    temp += to_string(this->cities.size()) + ",";
    temp += to_string(this->totalPopulation) + ",";
    temp += to_string(this->averageFitness) + ",";
    temp += to_string(this->avgAgeOfDeath) + ",";
    temp += to_string(this->avgDistBtwn2Points) + ",";
    for (int i = 0; i < this->cities.size(); ++i) {
        temp += to_string(i) + ",";
        temp += to_string(this->cities[i].totalPopulation) + ",";
        temp += to_string(this->cities[i].averageFitness) + ",";
        temp += to_string(this->cities[i].lifeExpectancy) + ",";
    }
    temp[temp.size() - 1] = '\n';
    data.push_back(temp);
}

void World::writeFile() {
    outFile.open(outFileName);
    //generator(immigration_rate, dim, birthrate), a(alpha), b(beta), c0(constant_of_influence), moving(moving), outFileName(fileName)
    //w(alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, m, fileName)
    string inputs = "Alpha,Beta,Birthrate,Constant of Influence, Immigration Rate, Dimensions, Moving Rate\n";
    inputs += to_string(this->alpha) + "," + to_string(this->beta) + "," + to_string(this->generator.birthrate) + "," + to_string(this->c0) + "," + to_string(this->generator.getC()) + "," + to_string(this->generator.getDim()) + "," +  to_string(this->moving) + "\n";
    string temp = "Date,Total Cities,Total Population,Average Fitness,Average Age of Death,Average Distance Between 2 Cities,";
    for (int i = 0; i < cities.size(); ++i)
        temp += "City ID,Total Population,Average Fitness,Life Expectancy,";
    
    temp[temp.size() - 1] = '\n';
    data.insert(data.begin(), temp);
    
    outFile << inputs;
    for (int i = 0; i < data.size(); ++i)
        outFile << data[i];
    
    outFile.close();
    return;
}
