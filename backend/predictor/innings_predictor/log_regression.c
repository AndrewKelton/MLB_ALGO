#include "log_regression.h"

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

double logistic_func(double x)
{
    return 1 / (1 + exp(-x));
}

double log_cost();

double logit(double P)
{
    return ln(P / (1 - P));
}

double diff_two_logit(double p_1, double p_2)
{
    return logit(p_1) - logit(p_2);
}

double odds(double p)
{
    return p / (1 - p);
}

double probability_odds(double P)
{
    return exp(logit(P)) / (1 + exp(logit(P)));
}

double cost(double * y, double * p, int n)
{
    double total_cost = 0.0;

    for (int i = 0; i < n; i++) {
        total_cost += y[i] * ln(p[i]) + (1 - y[i]) * ln(1 - p[i]);
    }
    return -total_cost;
}


// return summation of values
double summation(double * vals, int bound, int i) 
{
    if (bound == i) 
        return 0.0;
    
    return vals[i] + summation(vals, bound, i+1);
}

// Compute the gradient
void compute_gradient(double *X, double *y, double *p, double *grad, int n_samples, int n_features) 
{
    for (int j = 0; j < n_features; j++) {
        grad[j] = 0.0;
        for (int i = 0; i < n_samples; i++) {
            grad[j] += (y[i] - p[i]) * X[i * n_features + j];
        }
        grad[j] /= n_samples; // Average the gradient
    }
}

// Perform gradient descent
void gradient_descent(double *X, double *y, double *beta, int n_samples, int n_features, double alpha, int iterations) 
{
    double * p = malloc(n_samples * sizeof(double)); // Predicted probabilities
    double * grad = malloc(n_features * sizeof(double)); // Gradient

    for (int iter = 0; iter < iterations; iter++) {
        // Compute predictions
        for (int i = 0; i < n_samples; i++) {
            double z = 0.0;
            for (int j = 0; j < n_features; j++) {
                z += X[i * n_features + j] * beta[j];
            }
            p[i] = sigmoid(z);
        }

        // Compute gradient
        compute_gradient(X, y, p, grad, n_samples, n_features);

        // Update weights (parameters)
        for (int j = 0; j < n_features; j++) {
            beta[j] += alpha * grad[j];
        }
    }

    free(p);
    free(grad);
}

void stats_to_feats(stats_t * stats, double ** features)
{
    *features[curr_szn] = stats->current_season;
    *features[prev_szn] = stats->prev_season;
    *features[l_3] = stats->last_3_games;
    *features[l_1] = stats->last_game;
    *features[home_avg] = stats->home_avg;
    *features[away_avg] = stats->away_avg;
}

void prepare_dataset(games_node_t *games, double **X, double *y, int n_samples, int n_features) {
    games_node_t *current = games;
    int i = 0;

    while (current != NULL && i < n_samples) {
        game_t *game = current->game;

        // Extract features for home and away teams
        stats_to_feats(game->home->stats, &X[i * n_features]);
        y[i] = 1.0; // Example: label for home team scoring in the first inning

        i++;
        current = current->next;
    }
}

void train_model(games_node_t *games, int n_samples, int n_features, double alpha, int iterations) {
    // Allocate memory for feature matrix and label vector
    double *X = malloc(n_samples * n_features * sizeof(double));
    double *y = malloc(n_samples * sizeof(double));
    double *beta = malloc(n_features * sizeof(double));

    // Initialize beta to zero
    for (int j = 0; j < n_features; j++) {
        beta[j] = 0.0;
    }

    // Prepare the dataset
    prepare_dataset(games, &X, y, n_samples, n_features);

    // Train the model
    gradient_descent(X, y, beta, n_samples, n_features, alpha, iterations);

    // Output the trained parameters
    printf("Trained parameters:\n");
    for (int j = 0; j < n_features; j++) {
        printf("beta[%d] = %f\n", j, beta[j]);
    }

    free(X);
    free(y);
    free(beta);
}

double predict(double *features, double *beta, int n_features) 
{
    double z = 0.0;
    for (int j = 0; j < n_features; j++) 
        z += features[j] * beta[j];
    
    return sigmoid(z);
}