#include "response.h"
#include "constants.h"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

double ResponseGenerator::Smear(double value, double sigma) {
    normal_distribution<double> d(value, sigma);
    return d(gen);
}

double ResponseGenerator::GetDiffusionSigma(double z) {
    return sqrt(2 * K_BOLTZ * TEMP * THICKNESS * z / BIAS / ELECTRON_CHARGE);
}

double ResponseGenerator::GetCCE(double dz) {
    return exp(- dz * THICKNESS / (MU * TAU * BIAS));
}


ResponseGenerator::ResponseGenerator() {

    wp.ReadFile("./NiceGrid.dat");
    random_device rd;
    gen = mt19937(rd());
}

double ResponseGenerator::InitialCloudSize(double e) {
    return CLOUD_A * e * (1 - CLOUD_B/( CLOUD_C * e + 1));
}

double ResponseGenerator::GetPixelRelatedV(double x, double y, double z, int x_pixel, int y_pixel) {
    double x_eff = x - x_pixel * PX_SIZE;
    double y_eff = y - y_pixel * PX_SIZE;

    double H = PX_SIZE / 2.0;

    if (z == 0) {
        if ((x_eff > -H) && (x_eff < H) && (y_eff > -H) && (y_eff < H)) {
            return 1;
        } else {
            return 0;
        }
    }

    return wp.GetV(x_eff*1e-6, y_eff*1e-6, z*1e-6);
}

map<pair<int, int>, double> ResponseGenerator::Process(const vector<PhotonHit>& hits) {
    map<pair<int, int>, double> result;

    //simulation parameters
    int n_group_electrons = 10;
    int n_steps = 1;

    for (const auto& hit : hits) {

        //initial ionization and its fluctuations limited by the Fano factor
        double n_electrons = hit.energy / PAIR_ENERGY;

        n_electrons = Smear(n_electrons, sqrt(n_electrons)*FANO);


        int n_groups = n_electrons / n_group_electrons;

        double cloud_size = 0; //InitialCloudSize(hit.energy);

        double diffusion_sigma = 0; //GetDiffusionSigma(hit.z);


        for (int i_group = 0; i_group < n_groups; ++i_group) {
            double xi = Smear(hit.x, cloud_size);
            double yi = Smear(hit.y, cloud_size);
            double zi = Smear(hit.z, cloud_size);

            double xf = Smear(xi, diffusion_sigma);
            double yf = Smear(yi, diffusion_sigma);
            double zf = 0;



            double charge = n_electrons / n_groups;


            for (int i_step = 0; i_step < n_steps; ++i_step) {
                double x_prestep = xi + (xf - xi) * i_step / n_steps;
                double y_prestep = yi + (yf - yi) * i_step / n_steps;
                double z_prestep = zi + (zf - zi) * i_step / n_steps;

                double x_poststep = xi + (xf - xi) * (i_step + 1) / n_steps;
                double y_poststep = yi + (yf - yi) * (i_step + 1) / n_steps;
                double z_poststep = zi + (zf - zi) * (i_step + 1) / n_steps;

                for (int x_pixel = -1; x_pixel <= 1; ++x_pixel) {
                    for (int y_pixel = -1; y_pixel <= 1; ++y_pixel) {
                            double v_pre = GetPixelRelatedV(x_prestep, y_prestep, z_prestep, x_pixel, y_pixel);
                            double v_post = GetPixelRelatedV(x_poststep, y_poststep, z_poststep, x_pixel, y_pixel);
                            //if (x_pixel * x_pixel + y_pixel*y_pixel == 0) cout << x_pixel << " " << y_pixel << " " << v_post << " " << v_pre << endl;
                            result[{x_pixel, y_pixel}] += charge * (v_post - v_pre);
                    }
                }
                //float coef = uBias * mutau / (thickness  * d * 0.00000001);
                charge *= GetCCE(fabs(z_prestep - z_poststep));

            }


        }
    }
    return result;
}
