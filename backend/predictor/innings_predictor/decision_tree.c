#include "decision_tree.h"


// compare season avg
static bool szn_comp(double szn)
{
    if (szn >= LARGE)
        return false;
    return true;
}

// compare one game avg
static bool _1_comp(double last_game)
{
    if (last_game >= 1.0)
        return false;
    return true;
}

// compare 3 game avg
static bool _3_comp(double last_three)
{
    if (last_three >= 1.0 / 3)
        return false;
    return true;
}

// compare home/away avg
static bool stadium_comp(double stadium)
{
    return szn_comp(stadium);
}

// // compare rank *** always returns true ***
// static bool rank_comp(unsigned int rank)
// {
//     if (rank >= 15)
//         return true;
//     return false;
// }

extern d_tree_t * init_tree()
{
    d_tree_t * head = malloc(sizeof(d_tree_t));
    head->right = NULL;
    head->left = NULL;
    head->is_true = false;
    head->result = 0.0;
    head->data_comp = 0;
    return head;
}

// compare and create decision tree
extern void _t_decision_tree(d_tree_t * dt, stats_t * st, int dec)
{
    if (dec > 6)
        return;
    
    d_tree_t * next = malloc(sizeof(d_tree_t));
    next->left = NULL;
    next->right = NULL;
    bool is_right = false;
    
    switch (dec) {
        case curr_szn: 
            next->is_true = szn_comp(st->current_season);
            next->result = st->current_season;
            next->data_comp = curr_szn;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
                return;
            }
            // printf("%lf\n", dt->result);
            break;

        case prev_szn:
            next->is_true = szn_comp(st->current_season);
            next->result = st->current_season;
            next->data_comp = prev_szn;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;

        case l_3:
            next->is_true = _3_comp(st->last_3_games);
            next->result = st->last_3_games;
            next->data_comp = l_3;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;
            
        case l_1:
            next->is_true = _1_comp(st->last_game);
            next->result = st->last_game;
            next->data_comp = l_1;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;

        case home_avg:
            next->is_true = stadium_comp(st->home_avg);
            next->result = st->home_avg;
            next->data_comp = home_avg;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;

        case away_avg:
            next->is_true = stadium_comp(st->away_avg);
            next->result = st->away_avg;
            next->data_comp = away_avg;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;

        case rank:
            next->is_true = st->rank;
            next->result = 0;
            next->data_comp = rank;

            if (next->is_true) {
                dt->right = next;
                dt = dt->right;
                is_right = true;
            } else {
                dt->left = next;
                dt = dt->left;
            }
            // printf("%lf\n", dt->result);
            break;

        default:
            // printf("DEFAULT\n");
            return;
    }

    _t_decision_tree(dt, st, dec + 1);
}

extern void free_d_tree(d_tree_t * dt)
{
    if (!dt)
        return;
    if (dt->left)
        free_d_tree(dt->left);
    if (dt->right)
        free_d_tree(dt->right);
    free(dt);
}