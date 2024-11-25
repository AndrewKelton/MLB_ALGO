#ifndef _LOG_REGRESSION_H
#define _LOG_REGRESSION_H
#include <math.h>
#include "utilities.h"
#include "innings_instr.h"

#define N_FEATURES 6
#define ln(x) log(x)

double sigmoid(double x);

double logistic_func(double x);

double logit(double x);

double diff_two_logit(double p_1, double p_2);

double odds(double p);

double probability_odds(double P);

double cost(double * y, double * p, int n);

static double summation(double * vals, int bound, int i); 

void compute_gradient(double *X, double *y, double *p, double *grad, int n_samples, int n_features);

void gradient_descent(double *X, double *y, double *beta, int n_samples, int n_features, double alpha, int iterations);

void stats_to_feats(stats_t * stats, double ** features);

void prepare_dataset(games_node_t *games, double **X, double *y, int n_samples, int n_features);

void train_model(games_node_t *games, int n_samples, int n_features, double alpha, int iterations);

double predict(double *features, double *beta, int n_features);

#endif