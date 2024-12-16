#ifndef DECISION_TREE_H
#define DECISION_TREE_H
#include "utilities.h"
#include "innings_instr.h"
#include <stdbool.h>

#define LARGE 0.5
#define MAX 7


// decision tree created
typedef struct D_TREE d_tree_t;

typedef struct D_TREE {
    unsigned int data_comp;
    double result;
    bool is_true;
    d_tree_t * left;
    d_tree_t * right;
} d_tree_t;

// initialize decision tree
extern d_tree_t * init_tree();

// decision tree for team
extern void _t_decision_tree(d_tree_t * dt, stats_t * st, int num);

// free decision tree
extern void free_d_tree(d_tree_t * dt);

#endif