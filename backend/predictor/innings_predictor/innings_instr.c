#include "innings_instr.h"

// String map from team abbreviation to full name
struct StringMapEntry team_map[MAX_TEAMS] = {
    {"AZ", "Arizona"},
    {"ATL", "Atlanta"},
    {"BAL", "Baltimore"},
    {"BOS", "Boston"},
    {"CHC", "Chi Cubs"},
    {"CWS", "Chi Sox"},
    {"CIN", "Cincinnati"},
    {"CLE", "Cleveland"},
    {"COL", "Colorado"},
    {"DET", "Detroit"},
    {"HOU", "Houston"},
    {"KC", "Kansas City"},
    {"LAA", "LA Angels"},
    {"LAD", "LA Dodgers"},
    {"MIA", "Miami"},
    {"MIL", "Milwaukee"},
    {"MIN", "Minnesota"},
    {"NYM", "NY Mets"},
    {"NYY", "NY Yankees"},
    {"OAK", "Oakland"},
    {"PHI", "Philadelphia"},
    {"PIT", "Pittsburgh"},
    {"SD", "San Diego"},
    {"SEA", "Seattle"},
    {"SF", "SF Giants"},
    {"STL", "St. Louis"},
    {"TB", "Tampa Bay"},
    {"TEX", "Texas"},
    {"TOR", "Toronto"},
    {"WSH", "Washington"},
};

// search for team name in map
struct StringMapEntry * search_name(char * name)
{
    for (int i = 0; i < MAX_TEAMS; i++) 
        if (strcmp(team_map[i].value, name) == 0 || strcmp(team_map[i].key, name) == 0) 
            return &team_map[i];
    
    bail_with_error("team does not exist");

    return NULL;
}


// initialize team list
extern void init_team_list(teams_list_t ** teams_list)
{
    *teams_list = malloc(sizeof(teams_list_t));
    (*teams_list)->team = NULL;
    (*teams_list)->next = NULL;
}

// initialize teams list
extern teams_list_t * init_teams_list() 
{
    teams_list_t * teams_list = NULL;
    teams_list = read_stats(teams_list);

    teams_list_t * del = teams_list;
    teams_list = teams_list->next;

    free(del);

    return teams_list;
}

// initialize game structure
extern void init_game(game_t ** game)
{
    *game = malloc(sizeof(teams_list_t));
    (*game)->home = NULL;
    (*game)->away = NULL;
}

// debugging
void print_team(team_t * team)
{
    if (team == NULL || team->team_name == NULL || team->stats == NULL) 
        bail_with_error("no team data");
    
    printf("%s---%d\n", team->team_name->value, team->stats->rank);
}

// add team to entry
void add_team_entry(teams_list_t ** head, teams_list_t * node)
{
    if (*head == NULL)
        *head = node;
    else {
        teams_list_t * curr = *head;

        while (curr->next) 
            curr = curr->next;
        
        curr->next = node;
    }
}

// read in from csv created from python
teams_list_t * read_stats(teams_list_t * teams_list)
{
    init_team_list(&teams_list);

    teams_list_t * head = teams_list;
    
    FILE * file = fopen(INNINGS_FILE, "r");

    if (!file) 
        bail_with_error("file cannot open");

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char * token = strtok(line, ",");
        int i = 0;

        if (strcmp(token, "Rank") == 0) 
            continue;

        teams_list_t * curr = malloc(sizeof(teams_list_t));
        team_t * team = malloc(sizeof(team_t));
        stats_t * stats = malloc(sizeof(stats_t));

        while (token) {
            switch (i)
            {
                case 0:
                    stats->rank = atoi(token);
                    break;
                case 1:
                    team->team_name = search_name(token);
                    break;
                case 2: 
                    stats->current_season = atof(token);
                    break;
                case 3:
                    stats->last_3_games = atof(token);
                    break;
                case 4:
                    stats->last_game = atof(token);
                    break;
                case 5:
                    stats->home_avg = atof(token);
                    break;
                case 6:
                    stats->away_avg = atof(token);
                    break;
                case 7:
                    stats->prev_season = atof(token);
                    break;
                default:
                    continue;
            }

            token = strtok(NULL, ",");
            i++;
        }
        team->stats = stats;
        curr->team = team;
        curr->next = NULL;

        add_team_entry(&head, curr);
    }
    
    fclose(file);

    return head;
}

// return probability of getting no runs first inning
double return_prob(double stat)
{
    return 1.0 - stat;
}