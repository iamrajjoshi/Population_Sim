#include "World.h"
#include "Threadpool.h"

void simRunner(double alpha, double beta, double birthrate, double constant_of_influence, unsigned int immigration_rate, unsigned int dim, double m, string fileName) {
    World w(alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, m, fileName);
    w.advanceSimulation(500);
    return;
}

int main() {
    //alpha, beta, birthrate, constant of influence rate of immigration dimensions moving mechanic
    thread_pool tp(4);
    vector<future<void>> futs;
    ifstream myfile;
    myfile.open("G:\\My Drive\\Projects\\Research Project\\general7\\general7.txt");
    for (int i = 0; i < 1; i++) {
        double alpha, beta, birthrate, constant_of_influence,  immigration_rate, dim,  m;
        myfile >> alpha >> beta >> birthrate >> constant_of_influence >> immigration_rate >> dim >> m;
        
        string fileName = "out" + to_string(i + 134) + ".csv";
        futs.push_back(tp.submit(bind(&simRunner, alpha, beta, birthrate, constant_of_influence, immigration_rate, dim, m, fileName)));
    }
    myfile.close();

    for (auto& fut : futs)
        fut.get();

    return 0;
}
