
void tail_sim() {
    TH1D* spectrum = new TH1D("energy_spectrum", "Measured energy spectrum", 100, 0, 20);
    TH1D* v[4];
    TH1D* z_distr = new TH1D("z", "Depth distribution", 50, 0, 500);
    for(int i = 0; i < 4; ++i) {
        auto name = ("spectrum_" + to_string(i+1) + "px").c_str();
        auto title = (to_string(i+1) + "px clusters").c_str();
        v[i] = new TH1D(name, title, 100, 0, 20);
    }

    map<ClusterType, int> cluster_stats;
    PhotonGenerator pg;
    ResponseGenerator rg;

        for (long int i = 0; i < 100000; ++i) {
            auto photon_distr = pg.Generate(PHOTON_ENERGY);
            auto p = rg.Process(photon_distr);

            spectrum -> Fill(p.deposit-0.001);
            v[p.npixels-1] -> Fill(p.deposit-0.001);
            z_distr -> Fill(p.z);
            cluster_stats[p.cluster_type]++;
        }

    spectrum -> GetXaxis() -> SetTitle("Energy, keV");
    spectrum -> GetYaxis() -> SetTitle("Counts");
    z_distr -> GetXaxis() -> SetTitle("z, #mum");
    z_distr -> GetYaxis() -> SetTitle("Counts");
   // z_distr -> SetAxisRange(0.0, 100000.0, "Y");


    spectrum ->Draw();
     TCanvas* c0 = new TCanvas("c0", "Depth", 800, 600);

    z_distr -> Draw();

    TCanvas* c2 = new TCanvas("c2", "Different sizes", 1200, 800);
    c2 -> Divide(2,2);

    for(int i = 0; i < 4; ++i) {
        c2 -> cd(i+1);
        v[i] -> Draw();
    }

    TFile* f = new TFile("out.root", "update");
    spectrum -> Write();
    f->Close();

    for (auto it : cluster_stats) {
        cout << it.first << ": " << it.second << endl;
    }

    cout << "ready" << endl;

}

void printstats() {
    Double_t energies[15] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    Double_t oc_side[15];
    Double_t oc_corner[15];

    for (int i = 0; i < 15; ++i) {
        cout << energies[i] << " keV" << endl;
        auto stats = clsstats(energies[i]);
        for (auto it : stats) {
            cout << it.first << ": " << it.second << endl;
        }
        oc_side[i] = (Double_t)stats[ClusterType::OFF_CENTER_SIDE] / (100000 - stats[ClusterType::NOT_DETECTED]);
        oc_corner[i] = (Double_t)stats[ClusterType::OFF_CENTER_CORNER] / (100000 - stats[ClusterType::NOT_DETECTED]);


        cout << endl;
    }
    TGraph* g1 = new TGraph(15, energies, oc_side);
    TGraph* g2 = new TGraph(15, energies, oc_corner);
    g1->GetXaxis()->SetTitle("Energy [keV]");
    g1->GetYaxis()->SetTitle("Fraction");
    g1->SetTitle("OFF_CENTER cluster fraction");
    g1->SetLineColor(kRed);
    g1->SetLineWidth(2);
    g2->SetLineColor(kBlue);
    g2->SetLineWidth(2);

    TLegend * leg = new TLegend(0.14, 0.7, 0.3, 0.9);
    leg->AddEntry(g1, "OFF_CENTER_SIDE", "PL");
    leg->AddEntry(g2, "OFF_CENTER_CORNER", "PL");

    g1->Draw();
    leg->Draw("C");
    g2->Draw("C same");

}

void old_sim() {
    double energy = 40;
    double n_particles = 5000;

    TH1D* hist = new TH1D("b", "b", 100, 20, 45);
    TFile* f = new TFile("/home/daniil/Desktop/trd_plots/old.root", "recreate");

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
        hist->Fill(res.deposit);
        cout << i << endl;
    }
    hist->Write();
    f->Close();
}
