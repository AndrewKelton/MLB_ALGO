#ifndef _INNINGS_INSTR_H
#define _INNINGS_INSTR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

#define MAX_TEAMS 30
#define INNINGS_FILE "innings.csv"

// independent type variables
typedef enum {
    curr_szn, prev_szn,
    l_3, l_1,
    home_avg, away_avg,
    rank
} IND_type;

// Entry for a string map
struct StringMapEntry {
    const char * key;
    const char * value;
};

// stats for first inning scoring
typedef struct stats_s {
    double current_season; 
    double prev_season;
    double last_3_games; 
    double last_game; 
    double home_avg;
    double away_avg;
    unsigned int rank;
} stats_t;

typedef struct team_s {
    stats_t * stats; 
    struct StringMapEntry * team_name;
} team_t;

typedef struct teams_list_s {
    team_t * team;
    struct teams_list_s * next;
} teams_list_t;

typedef struct game_s {
    team_t * home;
    team_t * away;
} game_t;

typedef struct games_node_s {
    game_t * game;
    struct games_node_s * next;
} games_node_t;

struct StringMapEntry * search_name(char * name);

// initialize team list
extern void init_team_list(teams_list_t ** teams_list);

// initialize team list with data
extern teams_list_t * init_teams_list(); 

// initialize game structure
extern void init_game(game_t ** game);

teams_list_t * read_stats(teams_list_t * teams_list);

void print_team(team_t * team);

void add_team_entry(teams_list_t ** head, teams_list_t * node);

double return_prob(double stat);


#endif