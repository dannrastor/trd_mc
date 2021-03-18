#pragma once

#include "absorption.h"
#include <vector>



class ResponseGenerator {
public:
    ResponseGenerator();
    std::vector<std::vector<double>> Process(const std::vector<PhotonHit>& distr);

};
