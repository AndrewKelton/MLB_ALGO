#ifndef _MULTI_REGRESSION_H
#define _MULTI_REGRESSION_H
#include "linear_regression.h"
#include <math.h>
#include <cblas.h>
#include <lapacke.h>


// independent type variables
typedef enum {
    curr_szn, prev_szn,
    l_3, l_1,
    home_avg, away_avg,
    rank
} IND_type;

/* over or under 1 run is dependent */

void create_matrix(team_t * home, team_t * away, double **X, double **Y, int *n_samples, int *n_feats);

void compute_weights(double *W, double *X, double *Y, int n_samples, int n_features);

void transpose_matrix(double **result, double **matrix, int rows, int cols);

void multiply_matrices(double **result, double **A, double **B, int rowsA, int colsA, int colsB);

void multiply_matrix_vector(double *result, double **matrix, double *vector, int rows, int cols);

void invert_matrix(double *matrix, int n);

#endif