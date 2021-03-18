#include "response.h"
#include "constants.h"

using namespace std;

ResponseGenerator::ResponseGenerator() {
    wp.ReadFile("./NiceGrid.dat");
}

map<pair<int, int>, double> ResponseGenerator::Process(const vector<PhotonHit>& hits) {
     map<pair<int, int>, double> result;
}
