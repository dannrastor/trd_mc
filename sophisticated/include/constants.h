#pragma once

//detector parameters
constexpr double THR = 4.4; //keV
constexpr double NOISE = 80; //electrons
constexpr double PX_SIZE = 55.0; //um
constexpr double THICKNESS = 500.0; //um

constexpr double BIAS = 400.0;
constexpr double TEMP = 320.0; //K

//semicounductor parameters
constexpr double PAIR_ENERGY = 0.0042; //keV
constexpr double FANO = 0.14;

constexpr double ELECTRON_CHARGE = 1.6e-19;
constexpr double K_BOLTZ = 1.38e-23;

constexpr double MU = 3e11; // um2 / (V * s)
constexpr double TAU = 30e-9; //seconds





//absorption parameters
// ln(mu[cm^-1]) = A * ln(E[keV]) + B
//for E < 10 keV (see NIST)
constexpr double A1 = -2.658;
constexpr double B1 = 11.401;
//for E > 10 keV
constexpr double A2 = -2.735;
constexpr double B2 = 13.570;


//fluo parameters
constexpr double FLUO_RATE = 0.5;
constexpr double FLUO_BRANCH = 0.5;
constexpr double FLUO_DISTANCE_GA = 40.0;
constexpr double FLUO_DISTANCE_AS = 15.0;
constexpr double FLUO_ENERGY = 10.0;

//initial cloud parameters
//size = A*Energy*(1 - B/(C*energy+1)
constexpr double CLOUD_A = 1.03; // um/kev
constexpr double CLOUD_B = 0.98;
constexpr double CLOUD_C = 0.003; // 1/kev
