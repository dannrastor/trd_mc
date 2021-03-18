#include "absorption.h"
#include "constants.h"

#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

PhotonGenerator::PhotonGenerator() {
    random_device rd;
    gen = mt19937(rd());
    unity = uniform_real_distribution<double>(0, 1);
}

double PhotonGenerator::yield_xy() {
    return PX_SIZE * unity(gen);
}

double PhotonGenerator::abs_length(double energy) {
    double A = (energy < 10) ? A1 : A2;
    double B = (energy < 10) ? B1 : B2;
    double mu = exp(A * log(energy) + B);
    return 1 / mu * 10000;

}

double PhotonGenerator::depth_distribution(double z, double energy) {
    return exp(-1.0 * (THICKNESS - z) / abs_length(energy));
};

double PhotonGenerator::yield_z(double energy) {
    uniform_real_distribution<double> urd(0, 1);

    while(1) {
        double z = THICKNESS * unity(gen);
        double p = unity(gen);
        if (p < depth_distribution(z, energy)) {
            return z;
        }
    }
};

vector<PhotonHit> PhotonGenerator::Generate(double energy) {
    vector<PhotonHit> result;

    double x = yield_xy();
    double y = yield_xy();
    double z = yield_z(energy);

    double if_fluo = unity(gen);
    if (if_fluo < FLUO_RATE && energy > FLUO_ENERGY) {
        double path = (unity(gen) > FLUO_BRANCH) ? FLUO_DISTANCE_AS : FLUO_DISTANCE_GA;
        double phi = 2 * M_PI * unity(gen);
        double cos_theta = 2 * unity(gen) - 1;
        double xf = x + path * cos(phi) * cos_theta;
        double yf = y + path * sin(phi) * cos_theta;
        double zf = z + path * cos_theta;
        result.push_back({x, y, z, energy - FLUO_ENERGY});
        result.push_back({xf, yf, zf, FLUO_ENERGY});
    } else {
        result.push_back({x, y, z, energy});
    }
    return result;
}
