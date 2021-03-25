#include "test_runner.h"

#include "absorption.h"
#include "clusters.h"
#include "constants.h"
#include "response.h"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

void PrintChargeMap(const map<pair<int, int>, double>& m, ostream& out) {

    double total = 0;
    for (const auto& [k, v] : m) {
        total += v * PAIR_ENERGY;
    }

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            out << fixed << setw(6) << setprecision(2) << m.at({j, i}) * PAIR_ENERGY << " ";
        }
        out << endl;
    }
    cout << "total " << total << endl;
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

void test_response() {
    ResponseGenerator rg;
    PhotonHit h{0, 0, 100, 60};
    vector<PhotonHit> v;
    v.push_back(h);
    PrintChargeMap(rg.Process(v), cout);
}

int main() {
    TestRunner tr;
    //RUN_TEST(tr, test_smear);
    //RUN_TEST(tr, test_v);
    RUN_TEST(tr, test_response);
    //RUN_TEST(tr, test_cloudsize);
    return 0;
}
