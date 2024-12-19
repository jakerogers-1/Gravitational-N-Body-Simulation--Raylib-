#ifndef BH_H
#define BH_H

#include <stdlib.h>
#include "base.h"

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

metadata_BH sim_barnes_hut(Body *bodies, int bnum, int max_per_node, float delta_t);
void add_body(Body *added, Node *node, metadata_BH* mbh);
void free_bh(Node *node);
bool dfs_insert(Body *body, Node *node, metadata_BH *mbh);
void dfs_borders(Node *node, Rectangle *borders, int *count);
void dfs_info(Node *node, Rectangle *borders, Vector2 *cmasses, float *tmasses, int *count);

#endif