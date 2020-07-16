#include "World.h"


World::World(double alpha, double beta, double birthrate, double constant_of_influence, unsigned int immigration_rate, unsigned int dim, double m, string name) :
    generator(immigration_rate, dim, birthrate), a(alpha), b(beta),
    c0(constant_of_influence), date(0), m(m), totalPopulation(0), totalCities(0), averageFitness(0), outFileName(name) {
};

void World::initializeSimulation() {
    aod = 0;
    caod = 0;

    pair<unsigned int, unsigned int> p(generator.spawnFirstImmigrant());
    cityCenters.push_back(p);
    Person person(18, calculateFitness(18));
    cities.push_back(person);
    totalPopulation = 1;
    totalCities = 1;
    averageFitness = person.fitness;
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
                for (int uu = 0; uu < it; uu++) {
                    cities[i].kids.push_back(Person(0, 0));
                }
                if (generator.kidProbability() < generator.birthrate - it)
                    cities[i].kids.push_back(Person(0, 0));
            }
            if (cities[i].adults[j].age > calculateLifeExpectancy(i)) {
                aod = double((aod * caod + cities[i].adults[j].age)) / (caod + 1);
                //aod=(aod*caod+cities[i].adults[j].age)/(caod+1);
                int temp = cities[i].adults[j].age;
                caod++;
                cities[i].cityAgeofDeath = double((cities[i].cityAgeofDeath * cities[i].AODcity + cities[i].adults[j].age)) / (cities[i].AODcity + 1);
                cities[i].AODcity++;
                cities[i].totalDeaths++;
                cities[i].adults.erase(cities[i].adults.begin() + j);
                j--;
            }
        }

        cities[i].totalKids = cities[i].kids.size();
        if (cities[i].totalKids == 0)
            cities[i].averageKidAge = 0;
        else
        {
            cities[i].averageKidAge = 0;
            for (int j = 0; j < cities[i].totalKids; ++j)
                cities[i].averageKidAge += cities[i].kids[j].age;

            cities[i].averageKidAge = cities[i].averageKidAge / cities[i].totalKids;
        }
    }

    return;
}

unsigned int World::calculateLifeExpectancy(unsigned int index) {
    double y = (double(cities[index].averageFitness) / this->averageFitness) - 0.3;
    double output = roundl(114 * exp(y) / (exp(y) + 1));
    cities[index].lifeexpectancy = output;
    return output;
}

void World::advanceImmigrantMechanic() {
    //assigning immigrants to a city
    vector <pair<double, double>> immigrants(generator.spawnImmigrants());
    vector <unsigned int> places(cities.size(),0);
    for (int i = 0; i < immigrants.size(); ++i) {

        vector<double> influences;
        for (int j = 0; j < this->cities.size(); ++j)
            influences.push_back(calculateInfluence(j, immigrants[i]));

        long double total_influence = accumulate(influences.begin(), influences.end(), 0.0);
        if (isnan(total_influence)) {
            cout << "WE HAVE A BOOBOO" << endl;
            writeFile();
            return;
        }
        totalCities = cities.size();
        vector<double> probabilities(totalCities + 1, 0);

        probabilities[0] = 1;
        double baseline = 1 / total_influence;
        long double temp = 0.0;
        for (int j = 1; j < (influences.size() + 1); ++j) {
            //cout << total_influence << endl;
            long double aryval = influences[j - 1];
            temp = aryval;
            baseline += temp;
            probabilities[j] = baseline;
        }
        if (total_influence == 0) {
            places.push_back(1);
            cities.push_back(City(calculateFitness(18), 18));
            cityCenters.push_back(immigrants[i]);
            cities[cities.size() - 1].adults.emplace_back(Person(18, 31));
            cities[cities.size() - 1].totalAdults++;
        }
        else {
            //cout << "   " << baseline;
            double num = generator.rng(0, 1 + total_influence);

            for (int j = 0; j < probabilities.size(); ++j) {
                if (j == 0) {
                    if (num < probabilities[j]) {
                        places.push_back(1);
                        cities.push_back(City(calculateFitness(18), 18));
                        cityCenters.push_back(immigrants[i]);
                        cities[cities.size() - 1].adults.emplace_back(Person(18, 31));
                        cities[cities.size() - 1].totalAdults++;
                        break;
                    }
                }
                else if (probabilities[j - 1] < num && probabilities[j] >= num) {
                    places[j-1]++;
                    cities[j - 1].adults.emplace_back(Person(18, 31));
                    cities[j-1].totalAdults++;
                    break;
                }
                else if (j == probabilities.size() - 1) {
                    places[j - 1]++;
                    cities[j - 1].adults.emplace_back(Person(18, 31));
                    cities[j - 1].totalAdults++;
                    break;
                }
            }
        }

    }
    totalCities = cities.size();
    string temp;
    for (int i = 0; i < places.size(); ++i)
        temp = temp + to_string(places[i]) + ",";
    temp.pop_back();
    imiplace.push_back(temp);

    return;
}

double World::calculateInfluence(unsigned int index, pair<double, double> point) {
    double d = sqrt(pow((this->cityCenters[index].first - point.first), 2) + pow((this->cityCenters[index].second - point.second), 2));
    r[0] = (r[0] * r[1] + d) / (r[1] + 1);
    r[1]++;
    double influence = (c0 * pow(d, -b)) * pow(cities[index].totalAdults, a);
    if ((c0 * 10000) - 1 < influence)
        influence = c0 * 1000;
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
        for (int j = 0; j < cities[i].totalAdults; ++j)
            cities[i].averageFitness += cities[i].adults[j].fitness;
        if (cities[i].averageFitness == 0);
        else
            cities[i].averageFitness = cities[i].averageFitness / cities[i].totalAdults;

    }
    this->averageFitness = 0;
    int count1 = 0;
    for (int j = 0; j < cities.size(); ++j)
        if (cities[j].averageFitness) {
            averageFitness += cities[j].averageFitness;
            count1++;
        }
    ac = count1;
    averageFitness = averageFitness / count1;

    return;
}

void World::advanceMovingMechanic() {
    vector <unsigned int> places(cities.size(), 0);
    vector <Person> people;
    for (int i = 0; i < cities.size(); ++i) {
        for (int j = 0; j < cities[i].adults.size(); ++j) {
            if (cities[i].adults[j].fitness > (1 + (m / 100)) * cities[i].averageFitness) {
                people.push_back(cities[i].adults[j]);
                cities[i].adults.erase(cities[i].adults.begin() + j);
                j--;
            }

        }
    }


    vector<double> probabilities(totalCities, 0);
    double baseline = 0;

    for (int j = 0; j < cities.size(); ++j) {
        double temp = cities[j].averageFitness / averageFitness / cities.size();
        baseline += temp;
        probabilities[j] = baseline;
    }



    for (int i = 0; i < people.size(); ++i) {
        double num = generator.rng(0, baseline);
        for (int j = 0; j < probabilities.size(); ++j) {
            if (j == 0) {
                if (num < probabilities[j]) {
                    places[j]++;
                    cities[j].adults.emplace_back(people[i]);
                }
            }
            else if (probabilities[j - 1] < num && probabilities[j] >= num) {
                places[j]++;
                cities[j].adults.emplace_back(people[i]);
            }
        }
    }
    string temp;
    for (int i = 0; i < places.size(); ++i)
        temp = temp + to_string(places[i]) + ",";
    temp.pop_back();
    movplace.push_back(temp);
    //SET TOTAL ADULTS FOR EACH CITY
    //SET AVERAGE ADULT AGE FOR EACH CITY
    //SET TOTAL POPULATION
    for (int i = 0; i < cities.size(); ++i) {
        cities[i].totalAdults = cities[i].adults.size();
        if (cities[i].totalAdults == 0)
            cities[i].averageAdultAge = 0;
        else
        {
            cities[i].averageAdultAge = 0;
            for (int j = 0; j < cities[i].totalAdults; ++j)
                cities[i].averageAdultAge += cities[i].adults[j].age;

            cities[i].averageAdultAge = cities[i].averageAdultAge / cities[i].totalAdults;
        }
        cities[i].totalPopulation = cities[i].totalAdults + cities[i].totalKids;
    }
    this->totalPopulation = 0;
    totalkids = 0;
    for (int j = 0; j < cities.size(); ++j) {
        totalPopulation += cities[j].totalPopulation;
        totalkids += cities[j].totalKids;
    }
    return;
}

void World::advanceSimulation(unsigned int count) {
    initializeSimulation();
    for (int i = 0; i < count; ++i) {
        //cout << date << endl;
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
    temp += to_string(this->totalCities) + ",";
    temp += to_string(this->ac) + ",";
    temp += to_string(this->totalPopulation) + ",";
    temp += to_string(this->averageFitness) + ",";
    temp += to_string(this->aod) + ",";
    temp += to_string(this->r[0]) + ",";
    temp += to_string(this->totalkids) + ",";
    for (int i = 0; i < this->cities.size(); ++i) {
        temp += to_string(i) + ",";
        temp += to_string(this->cities[i].totalPopulation) + ",";
        temp += to_string(this->cities[i].totalKids) + ",";
        temp += to_string(this->cities[i].totalAdults) + ",";
        temp += to_string(this->cities[i].averageFitness) + ",";
        temp += to_string(this->cities[i].lifeexpectancy) + ",";
        temp += to_string(this->cities[i].totalDeaths) + ",";
        temp += to_string(this->cities[i].cityAgeofDeath) + ",";
        temp += to_string(this->cities[i].averageAdultAge) + ",";
        temp += to_string(this->cities[i].averageKidAge) + ",";
    }
    temp[temp.size() - 1] = '\n';
    data.push_back(temp);
}

void World::writeFile() {
    outFile.open(outFileName);
    string temp = "Date,Total Cities,Active Cities,Total Population,Average Fitness,AOD,R,Kids,";
    for (int i = 0; i < totalCities; ++i) {
        temp += "City ID,Total Population,Total Kids,Total Adults,Average Fitness,Life Expectancy,Number of People Dead,AOD,Average Adult Age,Average Kid Age,";
    }
    temp[temp.size() - 1] = '\n';

    data.insert(data.begin(), temp);

    for (int i = 0; i < data.size(); ++i) {
        outFile << data[i];
    }

    outFile.close();
    return;
}
