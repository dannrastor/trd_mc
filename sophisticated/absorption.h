#pragma once

#include <random>

struct PhotonHit {
    double x;
    double y;
    double z;
    double energy;
};

class PhotonGenerator {
private:
    double yield_xy();
    double abs_length(double energy);
    double depth_distribution(double z, double energy);
    double yield_z(double energy);
public:
    PhotonGenerator();
    std::vector<PhotonHit> Generate(double energy);
private:
    std::uniform_real_distribution<double> unity;
    std::mt19937 gen;
};
