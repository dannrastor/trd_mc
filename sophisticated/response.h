#pragma once

#include "absorption.h"
#include "weighting_potential.h"
#include <vector>
#include <utility>
#include <map>



class ResponseGenerator {
public:
    ResponseGenerator();
    std::map<std::pair<int, int>, double> Process(const std::vector<PhotonHit>& distr);
private:
    WeightingPotentialMap wp;
};
