#ifndef MUSCL_H
#define MUSCL_H

struct MUSCL {
	float **qf, **qfL, **qfR, **QcL, **QcR;
};

// Interpolation by muscl method
MUSCL muscl(float **lQc);

#endif
