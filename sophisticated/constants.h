#pragma once

//detector parameters
constexpr double THR = 4.2;
constexpr double PAIR_ENERGY = 0.0042;
constexpr double NOISE = 80; //electrons
constexpr double FANO = 0.14;
constexpr double PX_SIZE = 55.0;
constexpr double THICKNESS = 500.0;


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
