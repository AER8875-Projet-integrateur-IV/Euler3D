#ifndef SETUP_H
#define SETUP_H

struct Values {
	float *bol, *bor, *x;
	float **qf, **Qc;
};

Values setup();

#endif
