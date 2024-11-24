#include <math.h>
#include "innings_instr.h"
#include "utilities.h"
#include "innings_game_linker.h"

#define MAX_TEAMS 30

// initialize games list sorted for each game
games_node_t * init_games(const char * date) 
{
    teams_list_t * teams_list = init_teams_list();
    map_game_list * map_list = get_games_date(date);

    return link_all_games(teams_list, map_list);
}

/* remove games with rank under 15 
 * (lower rank == more likely to get a run in 1st inning) */
int remove_low_rank(games_node_t ** games_list)
{
    games_node_t * prev = NULL;
    games_node_t * curr = *games_list;
    
    int length = 0;

    while (curr) {
        game_t * game = curr->game;

        if (game->away->stats->rank >= 15 && game->home->stats->rank >= 15) {
            prev = curr;
            curr = curr->next;
            length++;
        }
        else {
            games_node_t * temp = curr; 
            curr = curr->next; 

            if (prev == NULL) {
                *games_list = curr; 
            } else {
                prev->next = curr; 
            }

            free(temp->game->away->stats);
            free(temp->game->home->stats);
            free(temp->game);
            free(temp);
        }
    }

    return length;
} 

// return summation of values
double summation(double * vals, int bound, int i) 
{
    if (bound == i) 
        return 0.0;
    
    return vals[i] + summation(vals, bound, i+1);
}

// return mean of vals
double calc_mean(double * vals, int length)
{
    return summation(vals, length, 0) / length;
}

// linear regression
// double linear_regression(double * x, double * y)
// {
//     double x_mean = 
// }

void linear_regression_all(games_node_t *games, int length) {
    // Allocate memory for X and Y values
    double *vals_x0 = malloc(sizeof(double) * length);
    double *vals_y0 = malloc(sizeof(double) * length);

    // Populate X and Y values
    int i = 0;
    while (games) {
        team_t *home = games->game->home;
        team_t *away = games->game->away;

        vals_x0[i] = home->stats->current_season; // Feature
        vals_y0[i] = away->stats->current_season; // Target
        i++;
        games = games->next;
    }

    // Calculate means
    double x_mean = calc_mean(vals_x0, length);
    double y_mean = calc_mean(vals_y0, length);

    // Compute weights (W) and intercept (b)
    double numerator = 0.0;
    double denominator = 0.0;
    for (int i = 0; i < length; i++) {
        double x_diff = vals_x0[i] - x_mean;
        double y_diff = vals_y0[i] - y_mean;
        numerator += x_diff * y_diff;
        denominator += x_diff * x_diff;
    }

    double W = numerator / denominator;
    double b = y_mean - W * x_mean;

    printf("Linear Regression Coefficients:\n");
    printf("  W (slope): %f\n", W);
    printf("  b (intercept): %f\n", b);

    // Free allocated memory
    free(vals_x0);
    free(vals_y0);
}


/*
void linear_regression_all(games_node_t * games, int length bounds)
{
    // linear regression vals
    double * vals_x0 = malloc(sizeof(double) * length);
    // double * vals_x1 = malloc(sizeof(double) * length);
    // double * vals_x2 = malloc(sizeof(double) * length);
    double * vals_y0 = malloc(sizeof(double) * length);
    // double * vals_y1 = malloc(sizeof(double) * length);
    // double * vals_y2 = malloc(sizeof(double) * length);

    int i = 0;
    while (games) {
        team_t * home = games->game->home;
        team_t * away = games->game->away;

        vals_x0[i] = home->stats->current_season;
        vals_y0[i++] = home->stats->current_season;
        
        games = games->next;
    }
}

*/

int main(/*int argc, char * argv[]*/) 
{ 
    games_node_t * games_list = init_games("2024-08-21");

    int length = remove_low_rank(&games_list);

    linear_regression_all(games_list, length);

    // double *X, *Y;
    // int n_samples, n_features;
    // prepare_data(games_list, &X, &Y, &n_samples, &n_features, length * 2);


    return 0;
}









/*
    while (games_list != NULL) {
            printf("%d\t%d\n", games_list->game->home->stats->rank, games_list->game->away->stats->rank);
            games_list = games_list->next;
    }
*/