

#include "absorption.h"
#include "clusters.h"
#include "constants.h"
#include "response.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;



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

    TH1D* hist = new TH1D("a", "a", 200, 0, 50);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/mono40kev_z500um.root", "recreate");

    mt19937 gen(random_device{}());
    uniform_real_distribution<double> coord(-27.5, 27.5);

    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        double x = coord(gen);
        double y = coord(gen);
        PhotonHit h{x, y, 500, energy};
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
    double n_particles = 5000;

    TH1D* hist = new TH1D("a", "a", 80, 0, 45);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/out_nolife.root", "recreate");


    PhotonGenerator pg;
    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        auto res = rg.Process(pg.Generate(energy));
        double sum = 0;
        for (const auto& it : res) {
            if (it.second * PAIR_ENERGY > THR) {
                sum += it.second * PAIR_ENERGY;
            }
        }
        hist->Fill(sum);
        cout << i << endl;
    }
    hist->Write();
    f->Close();
}

void test_high_z() {
    double energy = 40;
    double n_particles = 1000;


    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/testhighz.root", "recreate");

    mt19937 gen(random_device{}());
    uniform_real_distribution<double> coord(-27.5, 27.5);
    ResponseGenerator rg;

    for (double z = 500; z > 400; z -= 5) {

        auto h = to_string(int(z)).c_str();
        TH1D* hist = new TH1D(h, h, 200, 0, 50);

        for (int i = 0 ; i < n_particles; ++i) {
            double x = coord(gen);
            double y = coord(gen);
            PhotonHit h{x, y, z, energy};
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
            cout << z << " " << i << endl;
        }
        hist->Write();
    }


    f->Close();
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



int main() {
    //test_absorption();
    //make_energy_plot();
    //make_energy_plot_pg();
    //test_high_z();
    while(1) {
        test_response();
    }
    return 0;
}
