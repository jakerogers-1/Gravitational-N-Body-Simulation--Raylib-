#include "sim_naive.h"

void sim_naive(Body *bodies, int bnum, float delta_t)
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

            Vector2 force = Vector2Scale(Vector2Normalize(disp_vec), 
            GRAV_CONST * bodies[i].mass 
            * bodies[j].mass / (Vector2LengthSqr(disp_vec) + 0.1));

            net_force = Vector2Add(net_force, force);
        }
        bodies[i].vel = Vector2Add(bodies[i].vel, 
            Vector2Scale(net_force, delta_t / bodies[i].mass));

        bodies[i].pos = periodic_add(bodies[i].pos, 
            Vector2Scale(bodies[i].vel, delta_t));        
    }
}