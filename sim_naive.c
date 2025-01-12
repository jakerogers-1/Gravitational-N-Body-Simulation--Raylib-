#include "sim_naive.h"

void calc_force_naive(Body* bodies, int bnum) 
{
    for (int i = 0; i < bnum; i++) 
    {
        Vector2 net_force = Vector2Zero();
        Vector2 r1 = bodies[i].pos;

        for (int j = 0; j < bnum; j++) 
        {
            if (i == j) { continue; }

            Vector2 r2 = bodies[j].pos;
            Vector2 disp_vec = get_disp_vec(r2, r1);

            // +0.1 is to avoid unphysical singularity
            Vector2 force = Vector2Scale(Vector2Normalize(disp_vec), 
                GRAV_CONST * bodies[i].mass * bodies[j].mass / (Vector2LengthSqr(disp_vec) + 0.1));

            net_force = Vector2Add(net_force, force);
        }
        bodies[i].force = net_force;
    }
}

void sim_naive(Body *bodies, int bnum, float delta_t, Integrator integrator)
{
    calc_force_naive(bodies, bnum);
    int stage = 0;

    integrator.method(bodies, bnum, delta_t, &stage);

    if (integrator.type == EULER) { return; }

    else if (integrator.type == VEL_VERLET) 
    {     
        calc_force_naive(bodies, bnum);
        integrator.method(bodies, bnum, delta_t, &stage);
    }

    else if (integrator.type == YOSHIDA4)
    {
        for (int i = 0; i < 6; i++)
        {
            calc_force_naive(bodies, bnum);
            integrator.method(bodies, bnum, delta_t, &stage);
        }
    }
}