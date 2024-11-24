#ifndef _LINEAR_REGRESSION_H
#define _LINEAR_REGRESSION_H
#include "innings_instr.h"
#include "utilities.h"

// extern void prepare_data(games_node_t * games_list, double **X, double *Y, int *n_samples, int *n_features, int num_teams);

extern void compute_weights(double *W, double *X, double *Y, int n_samples, int n_features);

// return summation of vals
extern double summation(double * vals, int bound, int i); 

// return mean of vals
extern double calc_mean(double * vals, int length);

#endif