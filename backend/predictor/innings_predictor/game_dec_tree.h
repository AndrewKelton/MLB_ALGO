#ifndef GAME_DEC_H
#define GAME_DEC_H
#include "decision_tree.h"

/*  data structure holding game and their decision tree
    - uses linked list format between games
*/ 
typedef struct GAMES_TREE_S game_tree_t;

typedef struct GAMES_TREE_S {
    game_t * game;
    d_tree_t * home_tree;
    d_tree_t * away_tree;
    game_tree_t * next;
} game_tree_t;

game_tree_t * run_dec_all(games_node_t * games_list);

void comp_results_all(game_tree_t * game_dec_list);

#endif