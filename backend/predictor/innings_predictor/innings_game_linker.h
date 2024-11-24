#ifndef _INNINGS_GAME_LINKER_H
#define _INNINGS_GAME_LINKER_H
#include <sqlite3.h>
#include "innings_instr.h"
#include "utilities.h"

#define MAX 100 // max char bytes for sql
#define TABLE "games_"

typedef struct MAP_GAME_NODE_LIST {
    struct StringMapEntry * home_name;
    struct StringMapEntry * away_name;
} map_game_node_t;

typedef struct MAP_GAME_LIST {
    map_game_node_t * game_names;
    struct MAP_GAME_LIST * next;
} map_game_list;

static int callback(void *data, int argc, char **argv, char **azColName);

extern map_game_list * get_games_date(const char * date);

void init_games_map_list(map_game_list ** list);

void add_game_to_list(map_game_list ** head, map_game_node_t * game_node);

static game_t * link_team_to_game(map_game_node_t * map_node, teams_list_t * all_teams_list); 

extern games_node_t *link_all_games(teams_list_t *teams_list, map_game_list *map_list);

#endif
