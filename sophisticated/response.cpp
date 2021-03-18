#include "response.h"
#include "constants.h"

#include <algorithm>

using namespace std;

double ResponseGenerator::Smear(double value, double sigma) {
    normal_distribution<double> d(value, sigma);
    return d(gen);
}


ResponseGenerator::ResponseGenerator() {

    wp.ReadFile("./NiceGrid.dat");
    random_device rd;
    gen = mt19937(rd());
}
//
// map<pair<int, int>, double> ResponseGenerator::Process(const vector<PhotonHit>& hits) {
//      map<pair<int, int>, double> result;
//
//      int n_electrons = 10;
//      int n_segments = 10;
//
//
// }
