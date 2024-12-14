#include "matrix_comp.h"

static double * row_x;  // home stats array
static double * row_y;  // away stats array
static double ** dp;    // matrix

/* ======================================== */
/* === COMPARE DATA ======================= */
/* ======================================== */

/* compare stats in matrix 
 * uses dynamic programming with an approach similar to LCS
 * requires data structures not NULL
*/
void print_mat()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            printf("%lf ", dp[i][j]);
        }
        printf("\n");
    }
}

double comp_stats()
{
    for (int i = 1; i < TABLE_SIZE; i++) {
        for (int j = 1; j < TABLE_SIZE; j++) {
            if (i == rank || j == rank) 
                dp[i][j] = dp[i - 1][j] > dp[i][j-1] ? dp[i - 1][j] : dp[i][j-1];
            else if (row_x[i - 1] >= 0.5 || row_y[j - 1] >= 0.5)
                dp[i][j] = 1.0 + dp[i - 1][j - 1];
            else 
                dp[i][j] = dp[i - 1][j] > dp[i][j-1] ? dp[i - 1][j] : dp[i][j-1];
        }
    }

    return dp[MAX_SIZE][MAX_SIZE];
}

void clean_up()
{
    free(row_x); free(row_y);
    for (int i = 0; i < TABLE_SIZE; i++)
        free(dp[i]);
    free(dp);
    row_x = NULL;
    row_y = NULL;
    dp = NULL;
}


/* ======================================== */
/* === PREPARE DATA ======================= */
/* ======================================== */

/* compare stats and create a matrix 
like a LCS matrix */
void data_to_matrix(game_t * game)
{
    stats_t * home_stats = game->home->stats;
    stats_t * away_stats = game->away->stats;

    row_x = malloc(sizeof(double) * MAX_SIZE);
    row_y = malloc(sizeof(double) * MAX_SIZE);

    insert_stats(row_x, home_stats);
    insert_stats(row_y, away_stats);

    dp = malloc(sizeof(double*) * TABLE_SIZE);

    // initialize the matrix to hold 0s
    for (int i = 0; i < TABLE_SIZE; i++) {
        dp[i] = malloc(sizeof(double) * TABLE_SIZE);

        for (int x = 0; x < TABLE_SIZE; x++) {
            dp[i][x] = 0.0;
        }
    }
}

// insert stats to rows for comparison
void insert_stats(double * row, stats_t * s)
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        switch (i) {
            case curr_szn:
                row[i] = s->current_season;
                break;
            case prev_szn:
                row[i] = s->prev_season;
                break;
            case l_3:
                row[i] = s->last_3_games;
                break;
            case l_1:
                row[i] = s->last_game;
                break;
            case home_avg:
                row[i] = s->home_avg;
                break;
            case away_avg:
                row[i] = s->away_avg;
                break;
            case rank:
                row[i] = (double) s->rank;
                break;
            default:
                row[i] = 0.0;
        }
    }
}