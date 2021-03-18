#include "absorption.h"
#include <vector>

//detector parameters
#define THR 4.2
#define PAIR_ENERGY 0.0042
#define NOISE 80 //electrons
#define FANO 0.14
#define PX_SIZE 55.0
#define THICKNESS 500.0





class ResponseGenerator {
public:
    ResponseGenerator();
    std::vector<std::vector<double>> Process(const std::vector<PhotonHit>& distr);

};
