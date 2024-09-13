#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include<string.h>
#include<time.h>
#define FIRST_DAY '2024-08-14'

typedef struct LinkedList {
    int correct;
    struct LinkedList * next;
} wins_l;

static int callback(void *data, int argc, char **argv, char **azColName, wins_l * curr) {
    static int col = 2; // index of outcome of game in database 

    if (curr == NULL) {
        curr = malloc(sizeof(wins_l));
        curr->next = NULL;
    } else if (curr->next == NULL) {
        curr->next = malloc(sizeof(wins_l));
        curr->next->next = NULL;
        curr = curr->next;
    } 
    curr->correct = atoi(argv[2]);

    return 0;
}

int main(int argc, char * argv[]) {
   

    return 0;
}