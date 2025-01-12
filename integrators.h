#ifndef INTEGRATORS_H
#define INTEGRATORS_H

#include "base.h"

typedef void (*IntegratorMethod)(Body *body, int bnum, float delta_t, int *stage);

typedef enum
{
    EULER, 
    VEL_VERLET,
    YOSHIDA4,
} IntegratorType;

typedef struct 
{
    IntegratorType type;
    IntegratorMethod method;
} Integrator;


void integrate_euler(Body *body, int bnum, float delta_t, int *stage);
void integrate_velverlet(Body *body, int bnum, float delta_t, int *stage);
void integrate_yoshida4(Body *body, int bnum, float delta_t, int *stage);

Integrator select_integrator(IntegratorType type);

#endif