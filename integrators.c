#include "integrators.h"

void integrate_euler(Body *bodies, int bnum, float delta_t, int *stage) 
{
    for (int i = 0; i < bnum; i++) 
    {
        bodies[i].vel = Vector2Add(bodies[i].vel, 
            Vector2Scale(bodies[i].force, delta_t / bodies[i].mass));
        bodies[i].pos = periodic_add(bodies[i].pos, 
            Vector2Scale(bodies[i].vel, delta_t));
    }
}

void integrate_velverlet(Body *bodies, int bnum, float delta_t, int *stage) 
{
    if (*stage == 0)
    {
        for (int i = 0; i < bnum; i++) 
        {
            bodies[i].vel = Vector2Add(bodies[i].vel, 
                Vector2Scale(bodies[i].force, 0.5f * delta_t / bodies[i].mass));
            bodies[i].pos = periodic_add(bodies[i].pos, 
                Vector2Scale(bodies[i].vel, delta_t));
        }
        // return to caller for subsequent force calculation
        (*stage)++;
        return;
    }

    for (int i = 0; i < bnum; i++)
    {
        bodies[i].vel = Vector2Add(bodies[i].vel, 
            Vector2Scale(bodies[i].force, 0.5f * delta_t / bodies[i].mass));
   
    }
    (*stage) = 0;
}

const double w0 = - 1.70241438392;
const double w1 = 1.35120719196;
const double c1 = w1 / 2.0;
const double c4 = c1;
const double c2 = (w0 + w1) / 2.0;
const double c3 = c2;
const double d1 = w1;
const double d3 = d1;
const double d2 = w0;

void yoshida4_position(Body *bodies, int bnum, float delta_t, double cval)
{
    for (int i = 0; i < bnum; i++)
    {
        bodies[i].pos = periodic_add(bodies[i].pos, 
        Vector2Scale(bodies[i].vel, cval*delta_t));
    }
}

void yoshida4_velocity(Body *bodies, int bnum, float delta_t, double dval)
{
    for (int i = 0; i < bnum; i++)
    {
        bodies[i].vel = Vector2Add(bodies[i].vel, 
        Vector2Scale(bodies[i].force, dval*delta_t / bodies[i].mass));
    }
}

void integrate_yoshida4(Body *bodies, int bnum, float delta_t, int *stage) 
{
    switch (*stage)
    {
        case 0:
            yoshida4_position(bodies, bnum, delta_t, c1);
            (*stage)++;
            break;
        case 1:
            yoshida4_velocity(bodies, bnum, delta_t, d1);
            (*stage)++;
            break;
        case 2:
            yoshida4_position(bodies, bnum, delta_t, c2);
            (*stage)++;
            break;
        case 3:
            yoshida4_velocity(bodies, bnum, delta_t, d2);
            (*stage)++;
            break;
        case 4:
            yoshida4_position(bodies, bnum, delta_t, c3);
            (*stage)++;
            break;
        case 5:
            yoshida4_velocity(bodies, bnum, delta_t, d3);
            (*stage)++;
            break;
        case 6:
            yoshida4_position(bodies, bnum, delta_t, c4);
            *stage = 0;
            break;
    }
}

Integrator select_integrator(IntegratorType type) {
    IntegratorMethod method;
    switch (type) {
        case EULER:         method = integrate_euler;       break;
        case VEL_VERLET:    method = integrate_velverlet;   break;
        case YOSHIDA4:      method = integrate_yoshida4;    break;
        default:           
            type = EULER; 
            method = integrate_euler;       
            break;
    }
    return (Integrator){type, method};
}
