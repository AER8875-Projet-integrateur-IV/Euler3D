#include <iostream>
#include <string>
#include <math.h>

#include "cal_Q.h"
#include "parameters.h"
#include "setup.h"
#include "inner_ite.h"

using namespace std;

void cal_Q() {
    Values values = setup();
    values.Qc = inner_ite(values.Qc);
}