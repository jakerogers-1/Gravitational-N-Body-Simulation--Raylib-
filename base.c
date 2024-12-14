#include "base.h"

Vector2 periodic_add(Vector2 r1, Vector2 r2) 
{
    Vector2 res = Vector2Add(r1, r2);

    if (res.x > UNIVERSE_WIDTH/2)         { res.x -= UNIVERSE_WIDTH; }
    else if (res.x < -UNIVERSE_WIDTH/2)   { res.x += UNIVERSE_WIDTH; }

    if (res.y > UNIVERSE_HEIGHT/2)        { res.y -= UNIVERSE_HEIGHT; }
    else if (res.y < -UNIVERSE_HEIGHT/2)  { res.y += UNIVERSE_HEIGHT; }

    return res;
}

Vector2 get_disp_vec(Vector2 r1, Vector2 r2) 
{
    Vector2 disp = Vector2Subtract(r1, r2);
    
    if (disp.x > UNIVERSE_WIDTH/2)        { disp.x -= UNIVERSE_WIDTH; }
    else if (disp.x < -UNIVERSE_WIDTH/2)  { disp.x += UNIVERSE_WIDTH; } 

    if (disp.y > UNIVERSE_HEIGHT/2)       { disp.y -= UNIVERSE_HEIGHT; }
    else if (disp.y < -UNIVERSE_HEIGHT/2) { disp.y += UNIVERSE_HEIGHT; }

    return disp;
}