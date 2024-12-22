#include "sim_barnes_hut.h"
#include <stdio.h>

Node* create_empty_node(Rectangle border, int max_per_node)
{
    Node *node = (Node *)malloc(sizeof(Node));
    
    node->border = border;    
    node->bodies = malloc((max_per_node+1) * sizeof(Body));
    node->nbodies = 0;
    node->cmass = (Vector2){0.0f, 0.0f};
    node->tmass = 0.0f;
    
    for (int i = 0; i < 4; i++) { node->children[i] = NULL; }

    return node;
}

bool in_border(Vector2 pos, Rectangle border)
{
    if (pos.x >= border.x && pos.x < (border.x + border.width)
        && pos.y >= border.y && pos.y < (border.y + border.height))
    {
        return true;
    }
    return false;
}

void update_mass(Body *body, Node *node)
{
    float new_tmass = node->tmass + body->mass;

    node->cmass = Vector2Scale(
        Vector2Add(
            Vector2Scale(node->cmass, node->tmass), 
            Vector2Scale(body->pos, body->mass)
            ),
            1.0f/ new_tmass
        );

    node->tmass = new_tmass;
}

void branch_node(Node *node, metadata_BH *mbh)
{
    float new_w = node->border.width / 2.0;
    float new_h = node->border.height / 2.0;

    for (int i = 0; i < 4; i++)
    {
        float offset_x = (i % 2) * new_w;
        float offset_y = (i / 2) * new_h;  

        Rectangle border = {node->border.x + offset_x, 
            node->border.y + offset_y, new_w, new_h};

        node->children[i] = create_empty_node(border, mbh->max_per_node);
        
        for (int j = node->nbodies-1; j >= 0; j--)
        {
            if (dfs_insert(node->bodies[j], node->children[i], mbh))
            {
                node->bodies[j] = node->bodies[node->nbodies - 1];
                node->nbodies--;                    
            }
        }
    }
}

bool dfs_insert(Body *body, Node *node, metadata_BH *mbh)
{
    if (!in_border(body->pos, node->border))
    {        
        return false;
    }

    if (node->children[0] == NULL) // we're in an end node
    {
        node->bodies[node->nbodies] = body;
        node->nbodies++;
        update_mass(body, node);
        
        if (node->nbodies > mbh->max_per_node) { branch_node(node, mbh); }

        return true; 
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (dfs_insert(body, node->children[i], mbh)) 
        { 
            // can maybe rearrange this function
            update_mass(body, node);            
            return true; 
        }
    }
    return false;
}

void approx_bodies(Body *bodies, Node *node, Body *base, float theta, int *count)
{   
    if (node->children[0] == NULL)
    {
        for (int j = 0; j < node->nbodies; j++)
        {
            bodies[(*count)++] = *node->bodies[j];            
        }
        return;
    }
    
    // Update this to account for periodic boundaries
    float dist = Vector2Length(get_disp_vec(node->cmass, base->pos));

    if (dist == 0.0f || (node->border.width / dist) < theta)
    {    
        bodies[(*count)++] = (Body){node->cmass, Vector2Zero(), node->tmass};
        return;
    }
    
    for (int i = 0; i < 4; i++)
    {
        approx_bodies(bodies, node->children[i], base, theta, count);    
    }
}

void update_force(Body *body, Node *root, float theta, float delta_t, metadata_BH *mbh)
{
    Body *ap_bodies = malloc(MAX_NUM_BODIES * sizeof(Body));
    int count = 0;

    approx_bodies(ap_bodies, root, body, theta, &count);
    Vector2 net_force = Vector2Zero();

    for (int i = 0; i < count; i++)
    {
        // Don't calculate force wrt self
        if (Vector2Equals(ap_bodies[i].pos, body->pos)) { continue; }

        Vector2 disp_vec = get_disp_vec(ap_bodies[i].pos, body->pos);

        Vector2 force = Vector2Scale(Vector2Normalize(disp_vec), 
            GRAV_CONST * ap_bodies[i].mass * 
            body->mass / (Vector2LengthSqr(disp_vec) + 0.1));

        net_force = Vector2Add(net_force, force);
    }

    body->vel = Vector2Add(body->vel, Vector2Scale(net_force, 
        delta_t / body->mass));
    body->pos = periodic_add(body->pos, Vector2Scale(body->vel, delta_t));
}

metadata_BH sim_barnes_hut(Body *bodies, int bnum, int max_per_node, float delta_t)
{
    Node* root = create_empty_node((Rectangle){-UNIVERSE_WIDTH/2, 
        -UNIVERSE_HEIGHT/2, UNIVERSE_WIDTH, UNIVERSE_HEIGHT}, max_per_node);

    metadata_BH mbh = {root, 0, max_per_node};

    float theta = 0.5;

    // Rebuild the entire tree
    for (int i = 0; i < bnum; i++)
    {
        dfs_insert(&bodies[i], root, &mbh);
    }

    for (int i = 0; i < bnum; i++)
    {
        update_force(&bodies[i], root, theta, delta_t, &mbh);
    }

    return mbh;
}

void free_bh(Node *node)
{
    if (node == NULL) { return; }

    for (int i = 0; i < 4; i++)
    {
        free_bh(node->children[i]);
    }
    free(node);
}

// Unnecessary function but useful for visualisation in main.c
// Implementation of DFS for acquiring border information
void dfs_info(Node *node, Rectangle *borders, Vector2 *cmasses, float *tmasses, int *count)
{
    if (node == NULL) { return; }

    borders[*count] = node->border;
    cmasses[*count] = node->cmass;
    tmasses[*count] = node->tmass;
    
    (*count)++;

    for (int i = 0; i < 4; i++)
    {
        dfs_info(node->children[i], borders, cmasses, tmasses, count);
    }
}