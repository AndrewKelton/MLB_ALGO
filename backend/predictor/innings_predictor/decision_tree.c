#include "decision_tree.h"

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

extern void _t_decision_tree(d_tree_t * dt, stats_t * st, unsigned int dec)
{
    if (dec > rank)
        return;
    
    d_tree_t * next = malloc(sizeof(d_tree_t));
    next->left = NULL;
    next->right = NULL;
    
    switch (dec) {
        case curr_szn: 
            next->is_true = szn_comp(st->current_season);
            next->result = return_prob(st->current_season);
            next->data_comp = curr_szn;

            if (next->is_true)
                dt->right = next;
            else {
                dt->left = next;
                return;
            }
            break;

        case prev_szn:
            next->is_true = szn_comp(st->current_season);
            next->result = return_prob(st->current_season);
            next->data_comp = prev_szn;

            if (next->is_true)
                dt->right = next;
            else 
                dt->left = next;
            break;

        case l_3:
            next->is_true = _3_comp(st->last_3_games);
            next->result = return_prob(st->last_3_games);
            next->data_comp = l_3;

            if (next->is_true)
                dt->right = next;
            else 
                dt->left = next;
            break;
            
        case l_1:
            next->is_true = _1_comp(st->last_game);
            next->result = return_prob(st->last_game);
            next->data_comp = l_1;

            if (next->is_true)
                dt->right = next;
            else    
                dt->left = next;
            break;

        case home_avg:
            next->is_true = stadium_comp(st->home_avg);
            next->result = return_prob(st->home_avg);
            next->data_comp = home_avg;

            if (next->is_true)
                dt->right = next;
            else    
                dt->left = next;
            break;

        case away_avg:
            next->is_true = stadium_comp(st->away_avg);
            next->result = return_prob(st->away_avg);
            next->data_comp = away_avg;

            if (next->is_true)
                dt->right = next;
            else    
                dt->left = next;
            break;

        case rank:
            next->is_true = rank_comp(st->rank);
            next->result = 0;
            next->data_comp = rank;

            if (next->is_true)
                dt->right = next;
            else    
                dt->left = next;
            break;

        default:
            return;
    }

    decision_tree(next, st, dec + 1);
}

static bool szn_comp(double szn)
{
    if (szn >= LARGE)
        return false;
    return true;
}

static bool _1_comp(double last_game)
{
    if (last_game >= 1.0)
        return false;
    return true;
}

static bool _3_comp(double last_three)
{
    if (last_three >= 1.0 / 3)
        return false;
    return true;
}

static bool stadium_comp(double stadium)
{
    return szn_comp(stadium);
}

static bool rank_comp(unsigned int rank)
{
    if (rank >= 15)
        return true;
    return false;
}