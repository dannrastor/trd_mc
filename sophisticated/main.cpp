

#include "absorption.h"
#include "clusters.h"
#include "constants.h"
#include "response.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;


void PrintChargeMap(const map<pair<int, int>, double>& m, ostream& out) {

    double sum = 0;
    for (const auto& it : m) {
        if (it.second * PAIR_ENERGY > THR) {
            sum += it.second * PAIR_ENERGY;
        }
    }

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            out << fixed << setw(6) << setprecision(2) << m.at({j, i}) * PAIR_ENERGY << " ";
        }
        out << endl;
    }
    cout << "total " << sum << endl;
}



void test_smear() {
    ResponseGenerator rg;
    for (int i = 0; i < 10; ++i) cout << rg.Smear(0, 1) << endl;
}

void test_cloudsize() {
    ResponseGenerator rg;
    for (double e = 10; e <= 61; e += 10) {
        cout << e << " keV " << rg.InitialCloudSize(e) << " um" << endl;
    }
}

void test_v() {
    ResponseGenerator rg;
    for (double v = 0; v < 100; v+= 1) {
        cout << "z = " << v << " " <<rg.GetPixelRelatedV(0, 0, v, 0, 0) << endl;
    }
}

void test_gaas() {
    ResponseGenerator rg;
    for (double z = 0; z <= 500 ; z++) {
        //cout << "z = " << z << " CCE = " << rg.GetCCE(z) << " sig_diff = " << rg.GetDiffusionSigma(z) << endl;
        cout << rg.GetCCE(z) * (1 - rg.GetPixelRelatedV(0, 0, z, 0, 0)) << " ";
    }
}

void test_response() {
    ResponseGenerator rg;
    double z;
    cin >> z;
    PhotonHit h{0, 0, z, 60};
    vector<PhotonHit> v;
    v.push_back(h);
    cout << "cloud_size = " << rg.InitialCloudSize(h.energy) << endl;
    cout << "diffusion_sigma = " << rg.GetDiffusionSigma(h.z) << endl;
    PrintChargeMap(rg.Process(v), cout);
}

void test_depth() {
    ResponseGenerator rg;
}

void make_energy_plot() {
    double energy = 40;
    double n_particles = 5000;

    TH1D* hist = new TH1D("a", "a", 100, 20, 45);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/mono40kev_z400um.root", "recreate");

    mt19937 gen(random_device{}());
    uniform_real_distribution<double> coord(-27.5, 27.5);

    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        double x = coord(gen);
        double y = coord(gen);
        PhotonHit h{x, y, 400, energy};
        vector<PhotonHit> v;
        v.push_back(h);
        auto res = rg.Process(v);
        double sum = 0;
        for (const auto& it : res) {
            if (it.second * PAIR_ENERGY > THR) {
                sum += rg.Smear(it.second, 80) * PAIR_ENERGY;
            }
        }
        hist->Fill(sum);
        cout << i << endl;
    }
    hist->Write();
    f->Close();
}

void make_energy_plot_pg() {
    double energy = 40;
    double n_particles = 10000;

    TH1D* hist = new TH1D("a", "a", 200, 20, 45);
    TFile* f = new TFile("/home/daniil/Desktop/diploma_pics/spectrum.root", "recreate");


    PhotonGenerator pg;
    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        auto res = rg.Process(pg.Generate(energy));
        double sum = 0;
        for (const auto& it : res) {

            if (it.second * PAIR_ENERGY > THR) {
                sum += rg.Smear(it.second, NOISE) * PAIR_ENERGY;
            }
        }
        hist->Fill(sum);
        cout << i << endl;
    }
    hist->Write();
    f->Close();
}

void make_energy_plot_pg_clsstats() {
    double energy = 40;
    double n_particles = 10000;

    TH1D* hist = new TH1D("spectrum", "a", 200, 20, 45);
    TH1D* v[4];
    TH1D* z_distr = new TH1D("z", "Depth distribution", 100, 0, 500);

    for(int i = 0; i < 4; ++i) {
        auto name = ("spectrum_" + to_string(i+1) + "px").c_str();
        auto title = (to_string(i+1) + "px clusters").c_str();
        v[i] = new TH1D(name, title, 200, 25, 45);
    }

    TFile* f = new TFile("/home/daniil/Desktop/diploma_pics/spectrum_cls.root", "recreate");


    PhotonGenerator pg;
    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        auto res = rg.Process(pg.Generate(energy));
        double sum = 0;
        int cntr = 0;
        for (const auto& it : res) {

            if (it.second * PAIR_ENERGY > THR) {
                sum += rg.Smear(it.second, NOISE) * PAIR_ENERGY;
                cntr++;
            }
        }
        hist->Fill(sum);
        if (cntr > 0 && cntr < 4) {

            v[cntr-1]->Fill(sum);
        }
        cout << i << endl;
    }
    hist->Write();
    for(int i = 0; i < 4; ++i) {
        v[i] -> Write();
    }

    f->Close();
}

void make_wp_grid() {
    ofstream out("/home/daniil/Desktop/wp.dat");

    ResponseGenerator rg;
    for (double z = 0; z < 100; z+= 1) {
        for (double x = -35; x <= 35; x+=1) {
            out << rg.GetPixelRelatedV(x, 0, z, 0, 0) << " ";
        }
        out << endl;
    }
}

void test_absorption() {
    PhotonGenerator pg;
    int n_entries = 100000;

    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/test_abs.root", "recreate");

    TH2D* coords = new TH2D("xy", "xy", 50, -30, 30, 50, -30, 30);
    TH1D* depth = new TH1D("z", "z", 100, 0, 500);

    for (int i = 0; i < n_entries; ++i) {
        auto distr = pg.Generate(30);
        coords -> Fill(distr[0].x, distr[0].y);
        depth -> Fill(distr[0].z);
    }
    coords->Write();
    depth->Write();
    f->Close();


}

void test_wp_borders() {
    ResponseGenerator rg;
    for (double v = 0; v < 300; v+= 1) {
        cout << "x = " << v << " " <<rg.GetPixelRelatedV(v, v, 10, 0, 0) << endl;
    }
}

void make_energy_plot_am() {
    double energy = 59.5;
    double n_particles = 1000;

    TH1D* hist = new TH1D("spectrum", "a", 200, 0, 80);
    TH1D* v[4];


    for(int i = 0; i < 4; ++i) {
        auto name = ("spectrum_" + to_string(i+1) + "px").c_str();
        auto title = (to_string(i+1) + "px clusters").c_str();
        v[i] = new TH1D(name, title, 200, 0, 80);
    }

    //TFile* f = new TFile("/home/daniil/Desktop/diploma_pics/spectrum_cls_am.root", "recreate");


    PhotonGenerator pg;
    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        auto res = rg.Process(pg.Generate(energy));
        double sum = 0;
        int cntr = 0;
        for (const auto& it : res) {

            if (it.second * PAIR_ENERGY > THR) {
                sum += rg.Smear(it.second, NOISE) * PAIR_ENERGY;
                sum += rg.Smear(0, 300) * PAIR_ENERGY;
                cntr++;
            }
        }
        hist->Fill(sum);
        if (cntr > 0 && cntr <= 4) {

            v[cntr-1]->Fill(sum);
        }
        cout << i << endl;
    }
    //hist->Write();
    for(int i = 0; i < 4; ++i) {
        //v[i] -> Write();
    }

    //f->Close();
}

double calculate_cce_integral(int depth) {

    ResponseGenerator rg;

    double result = 0;
    double charge = 1;

    for (int idepth = depth; idepth > 0; idepth--) {
        charge *= rg.GetCCE(1.0);

        double v_pre = rg.GetPixelRelatedV(0, 0, idepth, 0, 0);
        double v_post = rg.GetPixelRelatedV(0, 0, idepth-1, 0, 0);

        result += charge * (v_post - v_pre);
    }

    return result;

}

int main() {
    auto start = steady_clock::now();

    //test_wp_borders();
    //test_absorption();
    make_energy_plot_am();
    //make_wp_grid();
    //
    // for (int i = 0; i < 500; ++i) {
    //     cout << i << " " << calculate_cce_integral(i) << endl;
    // }


    auto finish = steady_clock::now();
    cout << duration_cast<milliseconds>(finish - start).count() << "ms" << endl;
    return 0;
}
