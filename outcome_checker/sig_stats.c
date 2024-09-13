#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>
#define MAX 100

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

typedef struct Outcome {
    Game * game;
    char * pickname;
    int correct;
    // MostSigStat * mss;
} Outcome;

Game game[20];
int gamecount = 0;
Pitcher pitcher[40];
int pitchercount = 0;
Outcome outcomes[20];
int outcomecount = 0;

int numCorrect = 0;

int picked_tMSS[4] = {0};
int picked_pMSS[9] = {0};
int wrong_tMSS[4] = {0};
int wrong_pMSS[9] = {0};

// return percent of correctly predicted games
double percentCorrect() {
    return numCorrect / gamecount;
}

// check if home team is picked team
int isHome(Outcome o) {
    if (strcmp(o.pickname, o.game->home->name) == 0) return 1;
    return 0;
}

int overallMSS(int arr[], int size) {
    int mss = 0;
    for (int i = 1; i < size; i++) {
        if (arr[i] > arr[mss]) mss = i;
    }
    return mss;
}

/* 
    The next TWO functions are essentially the same 
    'wrong_findMSS' works for determining most
    significant stat for wrongly picked team while 
    'picked_findMSS' works for determing most 
    significant stat for correctly picked team
*/

// find most significant stat of incorrect game (picked team lost)
void wrong_findMSS(Outcome o) {
    Team * teamWon = (!isHome(o)) ? o.game->home : o.game->away;
    Team * teamLoss = (!isHome(o)) ? o.game->away : o.game->home;
    Pitcher * pitchWon = (!isHome(o)) ? o.game->homePitcher : o.game->awayPitcher;
    Pitcher * pitchLoss = (!isHome(o)) ? o.game->awayPitcher : o.game->homePitcher;

    // compare team stats
    if (teamWon->wins > teamLoss->wins) wrong_tMSS[0]++;
    if (teamWon->loss < teamLoss->loss) wrong_tMSS[1]++;
    if (teamWon->isDivLeader && !teamLoss->isDivLeader) wrong_tMSS[2]++;
    if (teamWon->wins / (teamWon->wins + teamWon->loss) > teamLoss->wins / (teamLoss->wins + teamWon->loss)) wrong_tMSS[3]++;

    // compare pitcher stats
    if (pitchWon->wins > pitchLoss->wins) wrong_pMSS[0]++;
    if (pitchWon->loss < pitchLoss->loss) wrong_pMSS[1]++;
    if (pitchWon->era < pitchLoss->era) wrong_pMSS[2]++;
    if (pitchWon->inningspitched > pitchLoss->inningspitched) wrong_pMSS[3]++;
    if (pitchWon->strikeouts > pitchLoss->strikeouts) wrong_pMSS[4]++;
    if (pitchWon->baseonballs < pitchLoss->baseonballs) wrong_pMSS[5]++;
    if (pitchWon->hits < pitchLoss->hits) wrong_pMSS[6]++;
    if (pitchWon->homeruns < pitchLoss->homeruns) wrong_pMSS[7]++;
    if (pitchWon->wins / (pitchWon->wins + pitchWon->loss) > pitchLoss->wins / (pitchLoss->wins + pitchLoss->loss)) wrong_pMSS[8]++;
}

// find most significant stat of correct game (picked team won)
void picked_findMSS(Outcome o) {
    Team * teamWon = (isHome(o)) ? o.game->home : o.game->away;
    Team * teamLoss = (isHome(o)) ? o.game->away : o.game->home;
    Pitcher * pitchWon = (isHome(o)) ? o.game->homePitcher : o.game->awayPitcher;
    Pitcher * pitchLoss = (isHome(o)) ? o.game->awayPitcher : o.game->homePitcher;

    // compare team stats
    if (teamWon->wins > teamLoss->wins) picked_tMSS[0]++;
    if (teamWon->loss < teamLoss->loss) picked_tMSS[1]++;
    if (teamWon->isDivLeader && !teamLoss->isDivLeader) picked_tMSS[2]++;
    if (teamWon->wins / (teamWon->wins + teamWon->loss) > teamLoss->wins / (teamLoss->wins + teamWon->loss)) picked_tMSS[3]++;

    // compare pitcher stats
    if (pitchWon->wins > pitchLoss->wins) picked_pMSS[0]++;
    if (pitchWon->loss < pitchLoss->loss) picked_pMSS[1]++;
    if (pitchWon->era < pitchLoss->era) picked_pMSS[2]++;
    if (pitchWon->inningspitched > pitchLoss->inningspitched) picked_pMSS[3]++;
    if (pitchWon->strikeouts > pitchLoss->strikeouts) picked_pMSS[4]++;
    if (pitchWon->baseonballs < pitchLoss->baseonballs) picked_pMSS[5]++;
    if (pitchWon->hits < pitchLoss->hits) picked_pMSS[6]++;
    if (pitchWon->homeruns < pitchLoss->homeruns) picked_pMSS[7]++;
    if (pitchWon->wins / (pitchWon->wins + pitchWon->loss) > pitchLoss->wins / (pitchLoss->wins + pitchLoss->loss)) picked_pMSS[8]++;
}

// check if game prediction was correct if so why? if not why?
void checkCorrect() {
    for (int i = 0; i < outcomecount; i++) {
        if (outcomes[i].correct == 1) {
            numCorrect++;
            picked_findMSS(outcomes[i]);
        } else {
            wrong_findMSS(outcomes[i]);
        }
    }
}

// find game in game array to connect to outcomes
Game * getGameFromID(int game_id) {
    for (int i = 0; i < gamecount; i++) {
        if (game_id == game[i].game_id) {
            return &game[i];
        }
    }
    return NULL;
}

// link pitchers to game
void linkPitchers() {
    int pcount = 0;
    for (int i = 0; i < gamecount; i++) {
        game[i].homePitcher = &pitcher[pcount++];
        game[i].awayPitcher = &pitcher[pcount++];
    }
}

// check if game has already been inserted to not confuse outcomes table with game table
int isSameGame(int game_id) {
    if (gamecount == 0) return 0;
    for (int i = 0; i < gamecount; i++) {
        if (game_id == game[i].game_id) {
            return 1;
        }
    }
    return 0;
}


/* ABOMINATION that gets data from sql database */
static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;

    for (i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "game_id") == 0 && !isSameGame(atoi(argv[i]))) {
            game[gamecount].game_id = atoi(argv[i]);
            game[gamecount].home = malloc(sizeof(Team)); game[gamecount].home->name = NULL;
            game[gamecount].away = malloc(sizeof(Team)); game[gamecount].away->name = NULL;
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
        } else if (strcmp(azColName[i], "game_id") == 0) {
            outcomes[outcomecount].game = getGameFromID(atoi(argv[i]));
        } else if (strcmp(azColName[i], "team_abbr") == 0) {
            outcomes[outcomecount].pickname = strdup(argv[i]);
        } else if (strcmp(azColName[i], "correct") == 0) {
            outcomes[outcomecount++].correct = atoi(argv[i]);
        }
    }
    return 0;
}
/* END OF ABOMINATION */


// query significant stat table in DB if not created
void createStatTable(sqlite3 * db, const char * tableName) {
    char sql[500];
    snprintf(sql, 500, "CREATE TABLE IF NOT EXISTS [%s] ("
        "date TEXT,"
        "team_wins INT,"
        "team_loss INT,"
        "div_leader INT,"
        "team_pct INT,"
        "pitcher_wins INT,"
        "pitcher_loss INT,"
        "pitcher_era INT,"
        "pitcher_inningspitched INT,"
        "pitcher_strikeouts INT,"
        "pitcher_baseonballs INT,"
        "pitcher_hits INT,"
        "pitcher_homeruns INT,"
        "pitcher_pct INT,"
        "picked_team_mss INT,"
        "picked_team_mss_count INT,"
        "picked_pitcher_mss INT,"
        "picked_pitcher_mss_count INT,"
        "wrong_team_mss INT,"
        "wrong_team_mss_count INT,"
        "wrong_pitcher_mss INT,"
        "wrong_pitcher_mss_count INT);", tableName);
    char * zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

// insert data into significant stat table
void insertData(sqlite3 * db, const char * tableName, char * date, int * team_stats, int * pitcher_stats, int correct_tMSS, int correct_pMSS, int incorrect_tMSS, int incorrect_pMSS) {
    char sql[1000]; int i = 0; int x = 0;
    snprintf(sql, 1000, "INSERT INTO [%s] (date, team_wins, team_loss, div_leader, team_pct, pitcher_wins, pitcher_loss, pitcher_era, pitcher_inningspitched, pitcher_strikeouts, pitcher_baseonballs, pitcher_hits, pitcher_homeruns, pitcher_pct, picked_team_mss, picked_team_mss_count, picked_pitcher_mss, picked_pitcher_mss_count, wrong_team_mss, wrong_team_mss_count, wrong_pitcher_mss, wrong_pitcher_mss_count) VALUES ('%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", tableName, date, team_stats[0], team_stats[1], team_stats[2], team_stats[3], pitcher_stats[0], pitcher_stats[1], pitcher_stats[2], pitcher_stats[3], pitcher_stats[4], pitcher_stats[5], pitcher_stats[6], pitcher_stats[7], pitcher_stats[8], correct_tMSS, picked_tMSS[correct_tMSS], correct_pMSS, picked_pMSS[correct_pMSS], incorrect_tMSS, wrong_tMSS[incorrect_tMSS], incorrect_pMSS, wrong_pMSS[incorrect_pMSS]);
    
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

// combine team stats to create list of both total datas (correct and incorrect picks)
int * combineTeamStats() {
    int * combined = malloc(sizeof(int) * 4);

    for (int i = 0; i < 4; i++) {
        combined[i] = picked_tMSS[i] + wrong_tMSS[i];
    }
    return combined;
}

// combine pitcher stats to create list of both total datas (correct and incorrect picks)
int * combinePitcherStats() {
    int * combined = malloc(sizeof(int) * 9);

    for (int i = 0; i < 9; i++) {
        combined[i] = picked_pMSS[i] + wrong_pMSS[i];
    }
    return combined;
}

int main(int argc, char * argv[]) {
    sqlite3 * db;
    char * zErrMsg = 0;
    int rc;
    char sql[MAX];
    const char * data = "Callback function called";
    rc = sqlite3_open("../data/lamp.db", &db);
    if (rc) {
        fprintf(stderr, "Cannot open db: %s\n", sqlite3_errmsg(db));
    }


    /* START GAME TABLE SEARCH */
    char gamesTable[MAX] = "games_";
    strcat(gamesTable, argv[1]);
    snprintf(sql, MAX, "SELECT * FROM [%s]", gamesTable);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    /* END GAME TABLE SEARCH */


    /* START PITCHER TABLE SEARCH */
    char pitchersTable[MAX] = "pitchers_";
    strcat(pitchersTable, argv[1]);
    snprintf(sql, MAX, "SELECT * FROM [%s]", pitchersTable);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    /* END PITCHER TABLE SEARCH */


    linkPitchers();


    /* START OUTCOMES TABLE SEARCH */
    char outcomesTable[MAX] = "outcomes_";
    strcat(outcomesTable, argv[1]);
    snprintf(sql, MAX, "SELECT * FROM [%s]", outcomesTable);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    /* END OUTCOMES TABLE SEARCH */


    // begin checking significant stats
    checkCorrect();

    int picked_team_mss = overallMSS(picked_tMSS, 4);
    int picked_pitcher_mss = overallMSS(picked_pMSS, 9);
    int wrong_team_mss = overallMSS(wrong_tMSS, 4);
    int wrong_pitcher_mss = overallMSS(wrong_pMSS, 9);
    int * combined_team_stats = combineTeamStats();
    int * combined_pitcher_stats = combinePitcherStats();

    createStatTable(db, "significant_stats");
    insertData(db, "significant_stats", argv[1], combined_team_stats, combined_pitcher_stats, picked_team_mss, picked_pitcher_mss, wrong_team_mss, wrong_pitcher_mss);

    sqlite3_close(db);

    return 0;
}