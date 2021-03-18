//absorption parameters
// ln(mu[cm^-1]) = A * ln(E[keV]) + B
//for E < 10 keV (see NIST)
#define A1 -2.658
#define B1 11.401
//for E > 10 keV
#define A2 -2.735
#define B2 13.570


//fluo parameters
#define FLUO_RATE 0.5
#define FLUO_BRANCH 0.5
#define FLUO_DISTANCE_GA 40.0
#define FLUO_DISTANCE_AS 15.0
#define FLUO_ENERGY 10.0


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
    double depth_distribution(double z);
    double yield_z();
public:
    PhotonGenerator();
    std::vector<PhotonHit> Generate(double energy);
private:
    std::uniform_real_distribution<double> unity;
    std::mt19937 gen;
};
