#ifndef NAIVE_H
#define NAIVE_H

#include "base.h"
#include "integrators.h"

void sim_naive(Body *bodies, int bnum, float delta_t, Integrator integrator);
void calc_force_naive(Body* bodies, int bnum);

#endif