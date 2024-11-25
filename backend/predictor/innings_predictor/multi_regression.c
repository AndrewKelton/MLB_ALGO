#include "multi_regression.h"


void create_matrix(team_t * home, team_t * away, double **X, double **Y, int *n_samples, int *n_feats)
{
    *n_samples = 2;
    *n_feats = 6;

    *X = malloc(2 * (*n_feats) * sizeof(double));
    *Y = malloc(2 * sizeof(double));

    int i = 0;
    (*X)[i * (*n_feats) + curr_szn] = home->stats->current_season;
    (*X)[i * (*n_feats) + prev_szn] = home->stats->prev_season;
    (*X)[i * (*n_feats) + l_3] = home->stats->last_3_games;
    (*X)[i * (*n_feats) + l_1] = home->stats->last_game;
    (*X)[i * (*n_feats) + home_avg] = home->stats->home_avg;
    (*X)[i * (*n_feats) + away_avg] = home->stats->away_avg;
    (*Y)[i] = (double)home->stats->rank;

    i++;
    (*X)[i * (*n_feats) + curr_szn] = away->stats->current_season;
    (*X)[i * (*n_feats) + prev_szn] = away->stats->prev_season;
    (*X)[i * (*n_feats) + l_3] = away->stats->prev_season;
    (*X)[i * (*n_feats) + l_1] = away->stats->last_3_games;
    (*X)[i * (*n_feats) + home_avg] = away->stats->last_game;
    (*X)[i * (*n_feats) + away_avg] = away->stats->away_avg;
    (*Y)[i] = (double)away->stats->rank;
}

void compute_weights(double *W, double *X, double *Y, int n_samples, int n_features) {
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

void transpose_matrix(double **result, double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j][i] = matrix[i][j];
        }
    }
}

void multiply_matrices(double **result, double **A, double **B, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            result[i][j] = 0;
            for (int k = 0; k < colsA; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiply_matrix_vector(double *result, double **matrix, double *vector, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        result[i] = 0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

void invert_matrix(double *matrix, int n) {
    int *ipiv = (int *)malloc(n * sizeof(int)); // Pivot indices
    int info;

    // Step 1: Perform LU decomposition
    info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, n, n, matrix, n, ipiv);
    if (info != 0) {
        fprintf(stderr, "Error in LU decomposition: %d\n", info);
        free(ipiv);
        return;
    }

    // Step 2: Invert the matrix
    info = LAPACKE_dgetri(LAPACK_ROW_MAJOR, n, matrix, n, ipiv);
    if (info != 0) {
        fprintf(stderr, "Error in matrix inversion: %d\n", info);
        free(ipiv);
        return;
    }

    free(ipiv); // Clean up
}