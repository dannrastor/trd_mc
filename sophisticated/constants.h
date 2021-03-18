//detector parameters
#define THR 4.2
#define PAIR_ENERGY 0.0042
#define NOISE 80 //electrons
#define FANO 0.14
#define PX_SIZE 55.0
#define THICKNESS 500.0


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
