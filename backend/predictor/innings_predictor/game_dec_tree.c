#include "game_dec_tree.h"

// run singular decision tree
static void dec_tree_team(d_tree_t * dt, team_t * t)
{
    // printf("HERE\n");
    _t_decision_tree(dt, t->stats, 0);
}

// run home/away decision trees
static void do_dec_tree(game_tree_t * gt)
{
    // printf("HERE\n");
    if (!gt) 
        return;
    // printf("HERE 11\n");
    dec_tree_team(gt->home_tree, gt->game->home);
    dec_tree_team(gt->away_tree, gt->game->away);
    do_dec_tree(gt->next);
}

// initialize tree
static game_tree_t * init_game_tree(game_t * game)
{
    game_tree_t * gt = malloc(sizeof(game_tree_t));
    gt->game = game;
    gt->away_tree = init_tree();
    gt->home_tree = init_tree();
    gt->next = NULL;

    return gt;
}

// initialize all trees
static game_tree_t * init_game_tree_all(games_node_t * games_list)
{
    game_tree_t * head = init_game_tree(games_list->game);

    game_tree_t * curr_t = head;
    games_node_t * curr_l = games_list->next;

    while (curr_l) {
        curr_t->next = init_game_tree(curr_l->game);
        curr_l = curr_l->next;
        curr_t = curr_t->next;
    }
    return head;
}

// run all decision trees
game_tree_t * run_dec_all(games_node_t * games_list)
{
    game_tree_t * head = init_game_tree_all(games_list);
    game_tree_t * curr = head;
    
    while (curr) {
        do_dec_tree(head);
        curr = curr->next;
    }
    return head;
}


// compare boolean array result
static double comp_res_bool_arr(bool * r_b)
{
    int count = 0;
    for (int i = 0; i < MAX; i++) 
        if (r_b[i])    
            count++;

    return (double) count / MAX;
}

// compare double array result
static double comp_res_db_arr(double * r_db)
{
    int count = 0;
    for (int i = 0; i < MAX; i++) {
        if (r_db[i] < 0)
            r_db[i] = r_db[i] * -1.0;
        if (r_db[i] > LARGE)
            continue;
        else 
            count++;
        printf("%lf\t",r_db[i]);
    }
    printf("\n");
    return (double) count / MAX;
}


// compare results from decision tree
static bool comp_results_game(game_tree_t * game_dec)
{
    if (!game_dec || !game_dec->away_tree || !game_dec->home_tree)
        return (double) 0;

    d_tree_t * home_dec = game_dec->home_tree;
    d_tree_t * away_dec = game_dec->away_tree;

    // arrays of results
    bool * res_b_arr = malloc(sizeof(bool) * MAX);
    double * res_diff = malloc(sizeof(double) * MAX);
    int i = 0;
    
    while (home_dec && away_dec) {
        printf("Home result: %lf, Away result: %lf\n", home_dec->result, away_dec->result);
       
       bool is_left = false;
        if (home_dec->left) {
            home_dec = home_dec->left;
            is_left = true;
        } else 
            home_dec = home_dec->right;

        if (away_dec->left) {
            away_dec = away_dec->left;
            is_left = true;
        } else
            away_dec = away_dec->right;
        
        if (!home_dec || !away_dec)
            break;

        if (is_left)
            res_b_arr[i] = false;
        else 
            res_b_arr[i] = true;

        // printf("%lf\t%lf\n", home_dec->result, away_dec->result);
        res_diff[i++] = home_dec->result - away_dec->result;
    }
    printf("\n");

    double res_b = comp_res_bool_arr(res_b_arr);
    double res_db = comp_res_db_arr(res_diff);
    free(res_b_arr);
    free(res_diff);

    printf("HOME: %s\tAWAY: %s\n", game_dec->game->home->team_name->key, game_dec->game->away->team_name->key);
    
    printf("RESULT: %lf\n", res_db + res_b / 2.0);

    FILE * fp = fopen("tmp.csv", "a");

    fprintf(fp, "%s, %s, %s, %lf, %lf, %lf\n", game_dec->game->home->team_name->key, game_dec->game->away->team_name->key, (res_db + res_b) / 2.0 < 0.8 || res_db != res_b ? "true" : "false", (res_b + res_db) / 2.0, res_b, res_db);
    fclose(fp);

    if ((res_b + res_db) / 2.0 < 0.8)
        return true;
    return false; 
}

static void free_tree_data(game_tree_t * gt)
{
    if (!gt)
        return;
    
    free_d_tree(gt->home_tree);
    free_d_tree(gt->away_tree);

    free_tree_data(gt->next);
    free(gt);
}

void comp_results_all(game_tree_t * game_dec_list)
{
    if (game_dec_list) {
        printf("%s\n", comp_results_game(game_dec_list) ? "true" : "false");
        comp_results_all(game_dec_list->next);
    }
    free_tree_data(game_dec_list);
}
