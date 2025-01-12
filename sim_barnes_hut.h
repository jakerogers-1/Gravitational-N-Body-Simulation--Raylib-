#ifndef BH_H
#define BH_H

#include <stdlib.h>
#include "base.h"
#include "integrators.h"

typedef struct Node {
    Rectangle border;
    struct Node *children[4];
    Body **bodies;
    int nbodies;
    Vector2 cmass; // centre of mass
    double tmass; // total mass
} Node;

typedef struct {
    Node* root; 
    int node_count;
    int max_per_node; // Max number of bodies to be contained in a given node
} metadata_BH;

metadata_BH sim_barnes_hut(Body *bodies, int bnum, int max_per_node, float theta, float delta_t, Integrator integrator);
void free_bh(Node *node);
bool dfs_insert(Body *body, Node *node, metadata_BH *mbh);
void dfs_info(Node *node, Rectangle *borders, Vector2 *cmasses, float *tmasses, int *count);

#endif