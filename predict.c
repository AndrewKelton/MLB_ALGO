#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>
#define MAX 100
#define MAXSCORE 7
int bestgame = -1;

typedef struct Team {
    char * name;
    int wins;
    int loss;
    int isDivLeader;
    int pitcherID;
} Team;

typedef struct Pitcher {
    int pitcherID;
    int wins;
    int loss;
    double era;
    double inningspitched;
    int strikeouts;
    double baseonballs;
    int hits;
    int homeruns;
} Pitcher;

typedef struct Game {
    Team * home;
    Team * away;
    Pitcher * homePitcher;
    Pitcher * awayPitcher;
    int sameDiv;
    int game_id;
} Game;

Game game[20]; 
int gamecount = 0;

Pitcher pitcher[40];
int pitchercount = 0;

int score = 0;
int h = 0, a = 0, both = 0;

/* TESTING */
void printGames() {
    int i = 0;

    while (game[i].home != NULL) {
        printf("GAME %d-- HOME: %s, AWAY: %s\n", i+1, game[i].home->name, game[i].away->name);
        i++;
    }
}
void printPitchers() {
    int i = 0;

    while (pitcher[i].pitcherID != -1) {
        printf("Pitcher id: %d\n", pitcher[i].pitcherID);
        i++;
    }
}
void printPitchersfromGame() {
    for (int i = 0; i < gamecount; i++) {
        printf("HOME PITCHER: %d -- AWAY PITCHER: %d\n", game[i].homePitcher->pitcherID, game[i].awayPitcher->pitcherID);
    }
}
/* TESTING */

// initialize all pitcher ids to -1
void initPitcherIDs() {
    for (int i = 0; i < 40; i++) {
        pitcher[i].pitcherID = -1;
    }
}

// set pitchers to games
void setPitchers() {
    int pcount = 0;
    for (int i = 0; i < gamecount; i++) {
        game[i].homePitcher = &pitcher[pcount++];
        game[i].awayPitcher = &pitcher[pcount++];
    }
}

// search for pitcherID in team games
void searchForPitcher(Pitcher * p) {
    for (int i = 0; game[i].home != NULL || i < 20; i++) {
        if (p->pitcherID == game[i].home->pitcherID) {
            game[i].homePitcher = p;
            return;
        } else if (p->pitcherID == game[i].away->pitcherID) {
            game[i].awayPitcher = p;
            return;
        }
    }
}

// calculate difference in probability
double diffInProb() {
    if (h > a) return (double) h-a;
    else if (h < a) return (double) a-h;
    return 0.0;
}

// compare teamstats for each game
void compTeamStats(Game g) {

    if (g.home->wins > g.away->wins && (g.home->wins / (g.home->wins + g.home->loss)) >= (g.away->wins / (g.away->wins + g.away->loss))) h++;
    else if (g.away->wins > g.home->wins && (g.away->wins / (g.away->wins + g.away->loss)) > (g.home->wins / (g.home->wins + g.home->loss))) a++;
    else both++;

    // comparing division leaders
    if (g.home->isDivLeader && !g.away->isDivLeader) h++;
        if (g.sameDiv) h++;
    else if (!g.home->isDivLeader && g.away->isDivLeader) a++;
        if (g.sameDiv) a++;
    else if (g.home->isDivLeader && g.away->isDivLeader) both++;

    if (h >= a) {
        score += h;
        score += both;
    } else {
        score -= a;
        score -= both;
    }
}

//compare pitcher stats
void compPitcherStats(Pitcher homeP, Pitcher awayP) {
    
    // compare hits allowed give 10 innings for padding
    if (homeP.hits <= awayP.hits && homeP.inningspitched <= awayP.inningspitched + 10) h++;
    else if (awayP.hits < homeP.hits && awayP.inningspitched <= homeP.inningspitched + 10) a++;
    else both++;

    // compare wins && win percentage
    if (homeP.wins >= awayP.wins && (homeP.wins / (homeP.wins + homeP.loss) >= awayP.wins / (awayP.wins + awayP.loss))) h++;
    else if (awayP.wins >= homeP.wins && (awayP.wins / (awayP.wins + awayP.loss) >= homeP.wins / (homeP.wins + homeP.loss))) a++;
    else both++;

    // compare strikouts
    if (homeP.strikeouts >= awayP.strikeouts) h++;
    else if (awayP.strikeouts > homeP.strikeouts) a++;
    else both++;

    // compare homeruns
    if (homeP.homeruns > awayP.homeruns) h--;
    else a--;

    // compare era
    if (homeP.era > awayP.era) h--;
    else a--;

    // compare win percentage only
    if (homeP.wins / (homeP.wins + homeP.loss) >= awayP.wins / (awayP.wins + awayP.loss)) h++;
    else a++;

/* ********************************************** */
    // NOT REALLY SURE RIGHT HERE                 //
    if (h >= a) {                                 //
        if (h > 0) score += h + both;             //
        else score -= h - both;                   //
    } else {                                      //
        if (a < 0) score += a + both;             //
        else score -= a - both;                   //
    }                                             //
/* ********************************************** */
}

// free allocated memory
void freeAllAllocated() {
    for (int i = 0; i < gamecount; i++) {
        free(game[i].home->name);
        free(game[i].away->name);
        free(game[i].home);
        free(game[i].away);
    }
}

// print best picks
void printBestTeams(Team * bestT, double * percents, double * diffs) {
    for (int i = 0; i < gamecount; i++) {
        printf("PICK FOR %s @ %s - - - %s %.2f%% - DIFF IN PROBAILITY %.2f%%\n", game[i].away->name, game[i].home->name, bestT[i].name, percents[i], diffs[i]);
    }
}

// query picks table in DB
void createPicksTable(sqlite3 * db, const char * tableName) {
    char sql[MAX];
    snprintf(sql, MAX, "CREATE TABLE IF NOT EXISTS [%s] ("
        "game_id INT,"
        "percent_win REAL,"
        "pick_name TEXT);", tableName);
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } // else {
    //     fprintf(stdout, "Table created successfully\n");
    // }
}

// insert data into picks table
void insertData(sqlite3 * db, const char * tableName, Team * pick, double * percents, int gamecount) {
    char sql[MAX];
    for (int i = 0; i < gamecount; i++) {
        snprintf(sql, MAX, "INSERT INTO [%s] (game_id, percent_win, pick_name) VALUES ('%d', '%.2f', '%s');", tableName, game[i].game_id, percents[i], pick[i].name);

        char *zErrMsg = 0;
        int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } // else {
        //     fprintf(stdout, "Data inserted successfully\n");
        // }
    }
}

/* EXCUSE THIS ABOMINATION OF A FUNCTION... it does as asked */
static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    // printf("%s: ", (const char*)data);
    
    // input game data
    for (i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "game_id") == 0) {
            game[gamecount].game_id = atoi(argv[i]);
            game[gamecount].home = malloc(sizeof(Team));
            game[gamecount].away = malloc(sizeof(Team));
        } else if (strcmp(azColName[i], "home_team") == 0) {
            game[gamecount].home->name = strdup(argv[i]);
        } else if (strcmp(azColName[i], "away_team") == 0) {
            game[gamecount].away->name = strdup(argv[i]);
        } else if (strcmp(azColName[i], "home_wins") == 0) {
            game[gamecount].home->wins = atoi(argv[i]);
        } else if (strcmp(azColName[i], "away_wins") == 0) {
            game[gamecount].away->wins = atoi(argv[i]);
        } else if (strcmp(azColName[i], "home_loss") == 0) {
            game[gamecount].home->loss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "away_loss") == 0) {
            game[gamecount].away->loss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "home_div_leader") == 0) {
            game[gamecount].home->isDivLeader = atoi(argv[i]);
        } else if (strcmp(azColName[i], "away_div_leader") == 0) {
            game[gamecount].away->isDivLeader = atoi(argv[i]);
        } else if (strcmp(azColName[i], "same_div") == 0) {
            game[gamecount].sameDiv = atoi(argv[i]);
        } else if (strcmp(azColName[i], "home_pitcher_id") == 0) {
            game[gamecount].home->pitcherID = atoi(argv[i]);
        } else if (strcmp(azColName[i], "away_pitcher_id") == 0) {
            game[gamecount++].away->pitcherID = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_id") == 0) {
            pitcher[pitchercount].pitcherID = atoi(argv[i]);
        } else if (strcmp(azColName[i], "wins") == 0) {
            pitcher[pitchercount].wins = atoi(argv[i]);
        } else if (strcmp(azColName[i], "losses") == 0) {
            pitcher[pitchercount].loss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "era") == 0) {
            pitcher[pitchercount].era = atof(argv[i]);
        } else if (strcmp(azColName[i], "inningspitched") == 0) {
            pitcher[pitchercount].inningspitched = atof(argv[i]);
        } else if (strcmp(azColName[i], "strikeouts") == 0) {
            pitcher[pitchercount].strikeouts = atoi(argv[i]);
        } else if (strcmp(azColName[i], "basoneballs") == 0) {
            pitcher[pitchercount].baseonballs = atof(argv[i]);
        } else if (strcmp(azColName[i], "hits") == 0) {
            pitcher[pitchercount].hits = atoi(argv[i]);
        } else if (strcmp(azColName[i], "homeruns") == 0) {
            pitcher[pitchercount++].homeruns = atoi(argv[i]);
        }
    }
    return 0;
}
/* END OF ABOMINATION */

int main(int argc, char * argv[]) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[MAX];
    const char * data = "Callback function called";

    rc = sqlite3_open("data/lamp.db", &db);
    if (rc) {
        fprintf(stderr, "Cannot open db: %s\n", sqlite3_errmsg(db));
    } // else {
    //     fprintf(stderr, "Opened successfully\n");
    // }

    /* START GAME TABLE SEARCH */
    char gamesTable[MAX] = "games_";
    strcat(gamesTable, argv[1]);
    snprintf(sql, MAX, "SELECT * FROM [%s]", gamesTable);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } // else {
    //     fprintf(stdout, "Operation done successfully\n");
    // }
    /* END GAME TABLE SEARCH */

    initPitcherIDs();

    /* START PITCHER TBALE SEARCH */
    char pitchersTable[MAX] = "pitchers_";
    strcat(pitchersTable, argv[1]);
    snprintf(sql, MAX, "SELECT * FROM [%s]", pitchersTable);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } // else {
    //     fprintf(stdout, "Operation done successfully\n");
    // }
    /* END PITCHER TABLE SEARCH */

    /* **** TESTING ****
    printGames();
    printPitchers();
    setPitchers();
       **** TESTING **** */

    for (int i = 0; i < pitchercount; i++) {
        searchForPitcher(&pitcher[i]);
    }

    // allocate memory for game data
    Team * bestTeams = malloc(sizeof(Team) * gamecount);
    double * percentages = malloc(sizeof(double) * gamecount);
    double * differencesInProb = malloc(sizeof(double) * gamecount);


    for (int i = 0; i < gamecount; i++) {
        compTeamStats(game[i]);
        compPitcherStats(*game[i].homePitcher, *game[i].awayPitcher);

        if (score >= 0) {
            bestTeams[i] = *game[i].home;
        } else {
            bestTeams[i] = *game[i].away;
        }

        if (score < 0) {
            score *= -1;
        }

        percentages[i] = (double) score / MAXSCORE * 100;
        differencesInProb[i] = diffInProb();

        score = 0, a = 0, h = 0, both = 0;
    }

    printBestTeams(bestTeams, percentages, differencesInProb);

    // insert picks into sql server
    char newTable[MAX] = "picks_";
    strcat(newTable, argv[1]);
    createPicksTable(db, newTable);
    insertData(db, newTable, bestTeams, percentages, gamecount);

    // free all data used
    freeAllAllocated();
    free(bestTeams);
    
    sqlite3_close(db); // close SQLDB

    return 0;
}