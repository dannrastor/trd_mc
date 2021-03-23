#pragma once

#include "absorption.h"
#include "weighting_potential.h"
#include <vector>
#include <utility>
#include <map>
#include <random>



class ResponseGenerator {
public:
    ResponseGenerator();
    std::map<std::pair<int, int>, double> Process(const std::vector<PhotonHit>& distr);
public:
    double Smear(double value, double sigma);
    double InitialCloudSize(double energy);
    double GetPixelRelatedV (double x, double y, double z, int x_pixel, int y_pixel);

    WeightingPotentialMap wp;
    std::mt19937 gen;
};
