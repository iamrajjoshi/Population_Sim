#include "Random.h"

Random::Random(double c, unsigned int dim, double br) : c(c), dim(dim), birthrate(br),
mt_rand((unsigned int)chrono::high_resolution_clock::now().time_since_epoch().count()) {};

double  Random::getC() { return this->c; }

unsigned int Random::getDim() { return this->dim; }

vector <pair<double, double>> Random::spawnImmigrants() {
    vector <pair<double, double>> out;
    uniform_real_distribution<double> unii(0, dim);
    for (int i = 0; i < c; ++i) {
        auto x = unii(mt_rand);
        auto y = unii(mt_rand);
        out.emplace_back(make_pair(x, y));
    }
    return out;
}

pair<double, double> Random::spawnFirstImmigrant() {
    uniform_real_distribution<double> unii(0, dim);
    auto x = unii(mt_rand);
    auto y = unii(mt_rand);
    return make_pair(x, y);
}

double Random::kidProbability() {
    uniform_real_distribution<double> unii(0, 1);
    return unii(mt_rand);
}

double Random::generate(double start, double end) {
    uniform_real_distribution<double> unii(start, end);
    return unii(mt_rand);
}
