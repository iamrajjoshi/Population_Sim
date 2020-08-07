#include "World.h"
#include "Threadpool.h"

void simRunner(int i, double alpha, double beta, double birthrate, double constOfInfluence, unsigned int immigrationRate, unsigned int dim, double moving, string fileName, unsigned int repeat) {
    cout << "Case " << i << " running..." << endl;
    World sim(alpha, beta, birthrate, constOfInfluence, immigrationRate, dim, moving, fileName);
    sim.simulate(repeat);
    cout << "Case " << i << " completed!" << endl;
    return;
}

int main() { //main function: takes inputs and outputs data to csv file
    //alpha, beta, birthrate, constant of influence, rate of immigration, dimensions, moving mechanic
    thread_pool tp;
    vector<future<void>> futs;
    ifstream myfile;
    unsigned int numCases;
    
    myfile.open("/Users/rj/Desktop/cases.txt");
    myfile >> numCases;
    cout << "Number of Cases: " << numCases << endl;
    for (int i = 0; i < numCases; i++) {
        double alpha, beta, birthrate, constOfInfluence,  immigrationRate, dim,  moving;
        myfile >> alpha >> beta >> birthrate >> constOfInfluence >> immigrationRate >> dim >> moving;
        
        string fileName = "/Users/rj/Desktop/CASE" + to_string(i) + ".csv";
        futs.push_back(tp.submit(bind(&simRunner, i, alpha, beta, birthrate, constOfInfluence, immigrationRate, dim, moving, fileName, 50)));
    }
    myfile.close();
    
    for (auto& fut : futs)
        fut.get();
    
    cout << "All Cases completed!" << endl;
    return 0;
}
