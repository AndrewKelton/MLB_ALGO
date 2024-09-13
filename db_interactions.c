#include "db_interactions.h"
#define MAX 100

void opend_db(sqlite3 * db) {
    int rc;
    rc = sqlite3_open("../data/lamp.db", &db);
    if (rc) {
        fprintf(stderr, "Cannot open db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
}

void select_all(sqlite3 * sql, const char * table, const char * date) {
    strcat(table, date);
    snprintf(sql, MAX, "SELECT * FROM [%s]", table);
}