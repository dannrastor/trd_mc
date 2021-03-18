#include "test_runner.h"

#include "absorption.h"
#include "clusters.h"
#include "constants.h"
#include "response.h"

#include <iostream>

void test_smear() {
    ResponseGenerator rg;
    for (int i = 0; i < 10; ++i) cout << rg.Smear(0, 1) << endl;
}

int main() {
    TestRunner tr;
    //RUN_TEST(tr, test_smear);
    return 0;
}
