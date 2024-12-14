#define BH_H
#ifndef BH_H

#include "base.h"

typedef struct {
    Rectangle border;
    Node     *children;
    Body     *bodies;
    double    cmass; // centre of mass
    double    tmass; // total mass
} Node;

void sim_barnes_hut();

#endif