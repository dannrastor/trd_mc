

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

void make_energy_plot() {
    double energy = 40;
    double n_particles = 5000;

    TH1D* hist = new TH1D("a", "a", 200, 0, 50);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/out.root", "recreate");

    mt19937 gen(random_device{}());
    uniform_real_distribution<double> coord(-27.5, 27.5);

    ResponseGenerator rg;

    for (int i = 0 ; i < n_particles; ++i) {
        double x = coord(gen);
        double y = coord(gen);
        PhotonHit h{x, y, 350, energy};
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
    double n_particles = 1000;

    TH1D* hist = new TH1D("a", "a", 200, 0, 50);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/out.root", "recreate");

    mt19937 gen(random_device{}());
    uniform_real_distribution<double> coord(-27.5, 27.5);


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
    make_energy_plot();
    return 0;
}
