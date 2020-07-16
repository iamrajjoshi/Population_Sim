#include "World.h"

void simRunner(double a, double b, double c, double d, double e, double f, double g, string fileName) {
    World w(a, b, c, d, e, f, g, fileName);
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
        double a, b, c, d, e, f, g;
        myfile >> a >> b >> c >> d >> e >> f >> g;
        
        string fileName = "out" + to_string(i + 134) + ".csv";
        futs.push_back(tp.submit(bind(&simRunner, a, b, c, d, e, f, g, fileName)));
    }
    myfile.close();

    for (auto& fut : futs)
        fut.get();

    return 0;
}
