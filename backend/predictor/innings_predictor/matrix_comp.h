#ifndef _MATRIX_COMP_H
#define _MATRIX_COMP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include "innings_instr.h"

#define MAX_SIZE 7
#define TABLE_SIZE 8

void print_mat();

void data_to_matrix(game_t * game);

void insert_stats(double * row, stats_t * s);

double comp_stats();

void clean_up();

#endif