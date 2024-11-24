#include "linear_regression.h"

// Prepare X and Y matrices for linear regression
// extern void prepare_data(games_node_t * games_list, double **X, double **Y, int *n_samples, int *n_features, int num_teams) 
// {
//     games_node_t * curr = games_list;
// 
//     *n_samples = num_teams;
//     *n_features = 6; // Example: Use 6 stats fields as features (current_season, prev_season, etc.)
// 
//     // Allocate matrices
//     *X = (double *)malloc(num_teams * (*n_features) * sizeof(double));
//     *Y = (double *)malloc(num_teams * sizeof(double));
//     
//     int i = 0;
//     while (curr != NULL) {
//         team_t *team = curr->game->home;
//         stats_t *stats = team->stats;
// 
//         // Fill features (X)
//         (*X)[i * (*n_features) + 0] = stats->current_season;
//         (*X)[i * (*n_features) + 1] = stats->prev_season;
//         (*X)[i * (*n_features) + 2] = stats->last_3_games;
//         (*X)[i * (*n_features) + 3] = stats->last_game;
//         (*X)[i * (*n_features) + 4] = stats->home_avg;
//         (*X)[i * (*n_features) + 5] = stats->away_avg;
// 
//         // Fill target (Y) - Example: Use rank as the target
//         (*Y)[i] = (double)stats->rank;
// 
//         curr = current->next;
//         i++;
//     }
// }


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

// Compute weights using the normal equation
extern void compute_weights(double *W, double *X, double *Y, int n_samples, int n_features) {
    // Allocate matrices
    double *X_transpose = (double *)malloc(n_samples * n_features * sizeof(double));
    double *XTX = (double *)malloc(n_features * n_features * sizeof(double));
    double *XTY = (double *)malloc(n_features * sizeof(double));

    // Step 1: Transpose X
    transpose_matrix(X_transpose, X, n_samples, n_features);

    // Step 2: Compute X^T X
    multiply_matrices(XTX, X_transpose, X, n_features, n_samples, n_features);

    // Step 3: Invert X^T X
    double *XTX_inv = (double *)malloc(n_features * n_features * sizeof(double));
    if (invert_matrix(XTX_inv, XTX, n_features) != 0) {
        printf("Matrix inversion failed.\n");
        free(X_transpose);
        free(XTX);
        free(XTY);
        return;
    }

    // Step 4: Compute X^T Y
    multiply_matrix_vector(XTY, X_transpose, Y, n_features, n_samples);

    // Step 5: Compute W = (X^T X)^(-1) X^T Y
    multiply_matrix_vector(W, XTX_inv, XTY, n_features, n_features);

    // Free memory
    free(X_transpose);
    free(XTX);
    free(XTX_inv);
    free(XTY);
}