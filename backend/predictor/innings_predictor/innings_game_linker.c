#include "innings_game_linker.h"

// initialize map list
void init_games_map_list(map_game_list **list)
{
    *list = malloc(sizeof(map_game_list));
    list = NULL;
}

// add game node to map list
void add_game_to_list(map_game_list **head, map_game_node_t *game_node)
{
    if (*head == NULL)
    {
        init_games_map_list(head);
        (*head)->game_names = game_node;
    }
    else if ((*head)->next == NULL && (*head)->game_names == NULL)
        (*head)->game_names = game_node;
    else
    {
        map_game_list *curr = *head;

        while (curr->next)
            curr = curr->next;

        init_games_map_list(&curr->next);

        curr = curr->next;
        curr->game_names = game_node;
    }
}

// interact with sql table
static int callback(void *data, int argc, char **argv, char **az_col_name)
{
    int i;

    map_game_list **list = (map_game_list **)data;

    map_game_node_t *curr_node = NULL;
    struct StringMapEntry *home_team = NULL;
    struct StringMapEntry *away_team = NULL;

    for (i = 0; i < argc; i++)
    {

        if (argv[i] != NULL && strcmp(az_col_name[i], "home_team") == 0)
        {
            home_team = search_name(argv[i]);

            if (home_team == NULL)
                bail_with_error("home is null");
        }
        else if (argv[i] != NULL && strcmp(az_col_name[i], "away_team") == 0)
        {
            away_team = search_name(argv[i]);

            if (away_team == NULL)
                bail_with_error("away is null");
        }

        if (home_team != NULL && away_team != NULL)
        {
            curr_node = malloc(sizeof(map_game_node_t));

            curr_node->home_name = home_team;
            curr_node->away_name = away_team;

            add_game_to_list(list, curr_node);

            home_team = NULL;
            away_team = NULL;
            curr_node = NULL;
        }
    }
    return 0;
}

// collect teams from date in sql db
extern map_game_list *get_games_date(const char *date)
{
    sqlite3 *db;
    char *z_err_msg = 0;
    int rc;
    char sql[MAX];

    rc = sqlite3_open("../data/lamp.db", &db);

    if (rc)
        bail_with_error(sqlite3_errmsg(db));

    char table[MAX] = TABLE;
    strcat(table, date);

    map_game_list *map_list = NULL;
    init_games_map_list(&map_list);

    snprintf(sql, MAX, "SELECT * FROM [%s]", table);

    rc = sqlite3_exec(db, sql, callback, (void *)&map_list, &z_err_msg);

    if (rc != SQLITE_OK)
    {
        bail_no_error(z_err_msg);
        sqlite3_free(z_err_msg);
    }

    return map_list;
}

// link team names for a game to their stats (return a game_t *)
static game_t *link_team_to_game(map_game_node_t *map_node, teams_list_t *all_teams_list)
{
    game_t *game = malloc(sizeof(game_t));
    game->away = NULL;
    game->home = NULL;

    while (all_teams_list)
    {
        if (all_teams_list->team->team_name == map_node->home_name)
            game->home = all_teams_list->team;

        else if (all_teams_list->team->team_name == map_node->away_name)
            game->away = all_teams_list->team;

        if (game->home && game->away)
            break;

        all_teams_list = all_teams_list->next;
    }

    return game;
}

// link all games to their names/stats return list of games
extern games_node_t *link_all_games(teams_list_t *teams_list, map_game_list *map_list)
{
    games_node_t *games_list = NULL;
    games_node_t *curr = games_list;

    while (map_list)
    {
        game_t *game = link_team_to_game(map_list->game_names, teams_list);

        curr = malloc(sizeof(games_node_t));

        curr->game = game;
        curr->next = games_list;
        games_list = curr;

        map_list = map_list->next;
    }

    curr = games_list;

/*
    while (curr)
    {
        printf("Home Team: %s\tAway Team: %s\n", curr->game->home->team_name->key, curr->game->away->team_name->key);
        curr = curr->next;
    }
*/

    return games_list;
}
