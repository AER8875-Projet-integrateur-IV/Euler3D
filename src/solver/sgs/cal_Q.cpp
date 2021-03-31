#include <iostream>
#include <math.h>
#include <string>

#include "cal_Q.h"
#include "inner_ite.h"
#include "parameters.h"
#include "setup.h"

using namespace std;

void cal_Q() {
	Values values = setup();
	values.Qc = inner_ite(values.Qc);
}