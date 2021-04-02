#include <iostream>
#include <math.h>
#include <string>

#include "solver/sgs/cal_Q.h"
#include "solver/sgs/inner_ite.h"
#include "solver/sgs/parameters.h"
#include "solver/sgs/setup.h"

using namespace std;

void cal_Q() {
	Values values = setup();
	values.Qc = inner_ite(values.Qc);
}