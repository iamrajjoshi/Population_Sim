#include "World.h"
#include "Threadpool.h"

void simRunner(int i, double alpha, double beta, double birthrate, double constant_of_influence, unsigned int immigration_rate, unsigned int dim, double moving, string fileName, unsigned int repeat) {
    World w(alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, moving, fileName);
    auto start = high_resolution_clock::now();
    w.simulate(repeat);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Simulation " << i << " Execution Time: " << duration.count() << " seconds" << endl;
    return;
}

int main() {
    //alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    thread_pool tp;
    vector<future<void>> futs;
    ifstream myfile;
    myfile.open("/Users/rj/Desktop/cases.txt");
    for (int i = 0; i < 1; i++) {
        double alpha, beta, birthrate, constant_of_influence,  immigration_rate, dim,  moving;
        myfile >> alpha >> beta >> birthrate >> constant_of_influence >> immigration_rate >> dim >> moving;
        
        string fileName = "/Users/rj/Desktop/CASE" + to_string(i) + ".csv";
        futs.push_back(tp.submit(bind(&simRunner, i, alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, moving, fileName, 500)));
    }
    myfile.close();
    
    auto start = high_resolution_clock::now();
    for (auto& fut : futs)
        fut.get();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Total Simulation Execution Time: " << duration.count() << " seconds" << endl;
    return 0;
}
