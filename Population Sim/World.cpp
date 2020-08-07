#include "World.h"

World::World(double alpha, double beta, double birthrate, double constOfInfluence, unsigned int immigrationRate, unsigned int dim, double moving, string fileName) : generator(immigrationRate, dim, birthrate), alpha(alpha), beta(beta), c0(constOfInfluence), moving(moving), outFileName(fileName) {};

void World::initializeSimulation() { //initialize the sumlation with 1 person spwaning at a random location and creating their own city
    date = 0;
    avgAgeOfDeath = 0;
    numberOfDeaths = 0;
    numInfluencesCalculations = 0;
    avgDistBtwn2Points = 0;
    pair<unsigned int, unsigned int> p(generator.spawnFirstImmigrant());
    cityCenters.push_back(p);
    cities.push_back(Person(18, calculateFitness(18)));
    totalPopulation = 1;
    averageFitness = cities[0].averageFitness;
    calculateLifeExpectancy(0);
    return;
}

void World::advanceAge() { //andvance the year for all the people within the simulation
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
                for (int uu = 0; uu < it; uu++) // birth of new kids
                    cities[i].kids.push_back(Person(0, 0));
                if (generator.kidProbability() < generator.birthrate - it)
                    cities[i].kids.push_back(Person(0, 0));
            }
            if (cities[i].adults[j].age > calculateLifeExpectancy(i)) { //death of anyone ove their repective life expectancy
                avgAgeOfDeath = double((avgAgeOfDeath * numberOfDeaths + cities[i].adults[j].age)) / (numberOfDeaths + 1);
                numberOfDeaths++;
                cities[i].adults.erase(cities[i].adults.begin() + j);
                j--;
            }
        }
    }
    return;
}

unsigned int World::calculateLifeExpectancy(unsigned int index) { //calculate the life expectancy of a city
    double y = (double(cities[index].averageFitness) / this->averageFitness) - 0.3;
    double output = roundl(114 * exp(y) / (exp(y) + 1));
    cities[index].lifeExpectancy = output;
    return output;
}

void World::advanceImmigrantMechanic() {
    //assigning immigrants to a city
    vector <pair<double, double>> immigrants(generator.spawnImmigrants());
    for (int i = 0; i < immigrants.size(); ++i) { //for each immigrant:
        
        vector<long double> influences;
        for (int j = 0; j < this->cities.size(); ++j) //calculate the influence of every city on the immigrant
            influences.push_back(calculateInfluence(j, immigrants[i]));
        
        long double total_influence = accumulate(influences.begin(), influences.end(), 0.0);
        total_influence += 1/total_influence;
       
        
        vector<long double> probabilities(cities.size() + 1, 0);
        for(int j = 1; j < probabilities.size(); ++j) //create a vector with size of total_influence and ranges based on they influence of a city
            probabilities[j] = probabilities[j-1] + (influences[j-1])/total_influence;
        
        long double num = generator.generate(0, probabilities[probabilities.size()-1]+(long double)1/total_influence);
        
        if (num > probabilities[probabilities.size() - 1]) { //if the random value falls within the last range and the immigrant starts a new city
            cities.push_back(Person(18, calculateFitness(18)));
            cityCenters.push_back(immigrants[i]);
        }
        
        else //if the immigrant is assigned to an existing city
            for (int j = 1; j < probabilities.size(); ++j) //assign immigrant to the city with the range for which the random value fell between
                if (probabilities[j - 1] < num && probabilities[j] >= num) {
                    cities[j - 1].adults.emplace_back(Person(18, 31));
                    break;
                }
    }
    
    return;
}

long double World::calculateInfluence(unsigned int index, pair<double, double> point) { //calculate influence of a city on an immigrant
    long double d = sqrt(pow((this->cityCenters[index].first - point.first), 2) + pow((this->cityCenters[index].second - point.second), 2));
    avgDistBtwn2Points = (avgDistBtwn2Points * numInfluencesCalculations + d) / (numInfluencesCalculations + 1);
    numInfluencesCalculations++;
    long double influence = (c0 * pow(d, -beta)) * pow(cities[index].adults.size(), alpha);
    return influence;
}

unsigned int World::calculateFitness(unsigned int age) { //calculate fitness of a person
    double d = -((double(age) - 45) * ((double(age) - 45)) / 625);
    double output = 100 * exp(d);
    return floor(output);
}

void World::advanceFitness() { //update the individual, city, and world fitnesses as the simulation progresses to a new year
    for (int i = 0; i < cities.size(); ++i) {
        //update individual fitness
        for (int j = 0; j < cities[i].adults.size(); ++j)//increment adult fitness
            cities[i].adults[j].fitness = calculateFitness(cities[i].adults[j].age);
        for (int j = 0; j < cities[i].kids.size(); ++j)//increment kids fitness
            cities[i].kids[j].fitness = calculateFitness(cities[i].kids[j].age);
        
        //update city fitness
        cities[i].averageFitness = 0;
        for (int j = 0; j < cities[i].adults.size(); ++j)
            cities[i].averageFitness += cities[i].adults[j].fitness;
        if (cities[i].averageFitness == 0);
        else cities[i].averageFitness = cities[i].averageFitness / cities[i].adults.size();
    }
    
    //update world fitness
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

void World::advanceMovingMechanic() { //Progress the simulation such that people who meet the required condition move to a differenct city
    vector<pair<Person,unsigned int>> people;

    for (int i = 0; i < cities.size(); ++i) {
           for (int j = 0; j < cities[i].adults.size(); ++j) {
               if (cities[i].adults[j].fitness * (1 + (moving / 100)) > averageFitness) { //create a vector with the people moving to a new city
                   people.push_back(make_pair(cities[i].adults[j], i));
                   cities[i].adults.erase(cities[i].adults.begin() + j);
                   j--;
               }
           }
       }
    
    vector<double> probabilities(cities.size(), 0);
    double baseline = 0;
    
    for (int j = 0; j < cities.size(); ++j) { //similar to immigrant mechanic, create a vector with ranges depending on the probability that a person moves to that city
        double temp = cities[j].averageFitness / averageFitness / cities.size();
        baseline += temp;
        probabilities[j] = baseline;
    }
    
    for (int i = 0; i < people.size(); ++i) { //assign all the people moving to a new city
        double num = generator.generate(0, baseline);
        for (int j = 0; j < probabilities.size(); ++j) {
            if (j == 0 && num < probabilities[j]) {
                if(people[i].second != j)
                    cities[j].adults.emplace_back(people[i].first);
                else {
                    unsigned int x = people[i].second;
                    while(x != people[i].second)
                        x = (unsigned int)generator.generate(0,cities.size()-1);
                    cities[x].adults.emplace_back(people[i].first);
                }
            }
                
            else if (probabilities[j - 1] < num && probabilities[j] >= num) {
                if(people[i].second != (j-1))
                    cities[j].adults.emplace_back(people[i].first);
                else {
                    unsigned int x = people[i].second;
                    while(x != people[i].second)
                        x = (unsigned int)generator.generate(0,cities.size()-1);
                    cities[x].adults.emplace_back(people[i].first);
                }
            }
        }
    }
    
    for (int i = 0; i < cities.size(); ++i) //update city total population
        cities[i].totalPopulation = (unsigned int)(cities[i].adults.size() + cities[i].kids.size());
    
    this->totalPopulation = 0;
    for (int j = 0; j < cities.size(); ++j)
        totalPopulation += cities[j].totalPopulation;
    return;
}


void World::simulate(unsigned int count) { //function that progresses to a new year for an inputted number of years
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

void World::addData() { //output calculated values/data to a csv file
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

void World::writeFile() { //write data to csv file in readable format
    outFile.open(outFileName);
    //generator(immigration_rate, dim, birthrate), a(alpha), b(beta), c0(constant_of_influence), moving(moving), outFileName(fileName)
    //w(alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, m, fileName)
    string inputs = "Alpha,Beta,Birthrate,Constant of Influence, Immigration Rate, Dimensions, Moving Rate\n";
    inputs += to_string(this->alpha) + "," + to_string(this->beta) + "," + to_string(this->generator.birthrate) + "," + to_string(this->c0) + "," + to_string(this->generator.getC()) + "," + to_string(this->generator.getDim()) + "," +  to_string(this->moving) + "\n";
    string temp = "Date,Total Cities,Total Population,Average Fitness,Average Age of Death,Average Distance Between 2 Points,";
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
