#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>

#define MAX 100 // used to allocate 100 bytes for chars ONLY
#define MAXSCORE 13 // maximum score team can get
#define SIGSTATTABLE "significant_stats"

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

typedef struct SigStats_t {
    int team_wins;
    int team_loss;
    int div_leader;
    int team_pct;
    int picked_mss;
    int picked_mss_count;
    int wrong_mss;
    int wrong_mss_count;
    struct SigStats_t * next;
    struct SigStats_t * back;
} SigStats_t;

typedef struct SigStats_p {
    int wins;
    int loss;
    int era;
    int innnings;
    int strikeouts;
    int baseonballs;
    int hits;
    int homeruns;
    int pct;
    int picked_mss;
    int picked_mss_count;
    int wrong_mss;
    int wrong_mss_count;
    struct SigStats_p * next;
    struct SigStats_p * back;
} SigStats_p;

// game data arrays
Game game[20]; 
int gamecount = 0;
Pitcher pitcher[40];
int pitchercount = 0;

// signifcant stats linked lists
SigStats_t * ssT_list = NULL;
SigStats_p * ssP_list = NULL;

int score = 0; // total score ** UNUSED **
int h = 0, a = 0, both = 0; // home, away, both teams stat scores


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
void printSigStats() {
    SigStats_t * tmp = ssT_list;
    while (tmp != NULL) {
        printf("THERE IS A SIGNIFICANT STAT\n");
        tmp = tmp->next;
    }
}
/* TESTING OVER */


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

// reverse linked lists to front
void reverseT() {
    if (ssT_list->back) {
        ssT_list = ssT_list->back;
        reverseT();
    }
}
void reverseP() {
    if (ssP_list->back) {
        ssP_list = ssP_list->back;
        reverseP();
    }
}

// calculate probability
double regProbability(int a) {
    return (double) a / MAXSCORE * 100;
}

/*  ********************* TEST ********************  */
// calculate difference in probability
double diffInProb() {
    if (h > a) return (double) h-a;
    else if (h < a) return (double) a-h;
    return 0.0;
}

// calculate factorial
double factorial(int i) {
    if (i <= 1) return 1;
    return (double) i * factorial(i-1);
}

/* THIS IS COUNTING FORMULA MESSED UP (NEVER USED) */
// calulate probability using probability forumula
double probability(int n, int b) {
    return factorial(n) / (factorial(b) * factorial(n-b));
}
/*  ****************************************************  */

// calculate prob of failure
double failure(int p) {
    return 1.0 - p;
}

// calculate diff in probabilites
double difference_events(double p1, double pBoth) {
    return p1 - pBoth;
}

/* WORKING ON THIS */
int getMssT() {
    int mssArr[4] = {0};
    SigStats_t * tmp = ssT_list;

    while (tmp != NULL) {
        for (int i = 0; i < 4; i++) {
            switch (i) {
                case 0:
                    mssArr[0] += tmp->team_wins;
                case 1:
                    mssArr[1] += tmp->team_loss;
                case 2:
                    mssArr[2] += tmp->div_leader;
                case 3:
                    mssArr[3] += tmp->team_pct;
                default:
                    break;
            }
        }
        tmp = tmp->next;
    }

    int largest = 0; int idx = 0;
    for (int i = 1; i < 4; i++) {
        if (mssArr[i] > mssArr[idx]) {
            largest = mssArr[i]; idx = i;
        }
    }
    return idx;
}

int decodeMssT(Game g) {
    int idx = getMssT();

    switch (idx) {
        case 0:
            if (g.home->wins > g.away->wins) return 1;
            else if (g.home->wins < g.away->wins) return -1;
            else return 0;
        case 1:
            if (g.home->loss < g.away->loss) return 1;
            else if (g.home->loss > g.away->loss) return -1;
            else return 0;
        case 2:
            if (g.home->isDivLeader && !g.away->isDivLeader) return 1;
            else if (!g.home->isDivLeader && g.away->isDivLeader) return -1;
            else if (g.home->isDivLeader && g.away->isDivLeader) return 0;
            else return 2;
        case 3:
            if (g.home->wins / (g.home->wins + g.home->loss) >= g.away->wins / (g.away->wins + g.away->loss)) return 1;
            else if (g.home->wins / (g.home->wins + g.home->loss) < g.away->wins / (g.away->wins + g.away->loss)) return -1;
            else return 0;
        default:
            return 2;
    }
    return 2;
}

// compare teamstats for each game
void compTeamStats(Game g) {
    // switch (decodeMssT(g)) {
    //     case -1:
    //         a++;
    //     case 0:
    //         both++, a++, h++;
    //     case 1:
    //         h++;
    // }

    // compare wins && losses
    if (g.home->wins > g.away->wins) h++;
    else if (g.home->wins < g.away->wins) a++;
    else both++, a++, h++;
    if (g.home->loss < g.away->loss) h++;
    else if (g.home->loss > g.away->loss) a++;
    else both++, a++, h++;

    // compare team_win_pct
    if (g.home->wins > g.away->wins && (g.home->wins / (g.home->wins + g.home->loss)) >= (g.away->wins / (g.away->wins + g.away->loss))) h++;
    else if (g.away->wins > g.home->wins && (g.away->wins / (g.away->wins + g.away->loss)) > (g.home->wins / (g.home->wins + g.home->loss))) a++;
    else both++, a++, h++;

    // comparing division leaders
    if (g.home->isDivLeader && !g.away->isDivLeader) h++;
        if (g.sameDiv) h++;
    else if (!g.home->isDivLeader && g.away->isDivLeader) a++;
        if (g.sameDiv) a++;
    else if (g.home->isDivLeader && g.away->isDivLeader) both++, a++, h++;

    // if (h >= a) {
    //     score += h;
    //     score += both;
    // } else {
    //     score -= a;
    //     score -= both;
    // }
}

//compare pitcher stats
void compPitcherStats(Pitcher homeP, Pitcher awayP) {
    
    // compare number of innings pitched (more experienced pitcher)
    if (homeP.inningspitched > awayP.inningspitched) h++;
    else if (homeP.inningspitched < awayP.inningspitched) a++;
    else both++, a++, h++;

    // compare hits allowed give 10 innings for padding
    if (homeP.hits <= awayP.hits && homeP.inningspitched <= awayP.inningspitched + 10) h++;
    else if (awayP.hits < homeP.hits && awayP.inningspitched <= homeP.inningspitched + 10) a++;
    else both++, a++, h++;

    // compare wins && win percentage
    if (homeP.wins >= awayP.wins && (homeP.wins / (homeP.wins + homeP.loss) >= awayP.wins / (awayP.wins + awayP.loss))) h++;
    else if (awayP.wins >= homeP.wins && (awayP.wins / (awayP.wins + awayP.loss) >= homeP.wins / (homeP.wins + homeP.loss))) a++;
    else both++, a++, h++;

    // compare strikouts
    if (homeP.strikeouts >= awayP.strikeouts) h++;
    else if (awayP.strikeouts > homeP.strikeouts) a++;
    else both++, a++, h++;

    // compare homeruns
    if (homeP.homeruns > awayP.homeruns) a++;
    else if (homeP.homeruns < awayP.homeruns) h++;
    else both++, a++, h++;

    // compare era
    if (homeP.era > awayP.era) a++;
    else if (homeP.era < awayP.era) h++;
    else both++, a++, h++;

    // compare win percentage
    if (homeP.wins / (homeP.wins + homeP.loss) > awayP.wins / (awayP.wins + awayP.loss)) h++;
    else if (homeP.wins / (homeP.wins + homeP.loss) < awayP.wins / (awayP.wins + awayP.loss)) a++;
    else both++, a++, h++;

    // compare wins only
    if (homeP.wins > awayP.wins) h++;
    else if (homeP.wins < awayP.wins) a++;
    else both++, a++, h++;

    // comeback to this 
/* ********************************************** */
    // if (h >= a) {                                 //
    //     if (h > 0) score += h + both;             //
    //     else score -= h - both;                   //
    // } else {                                      //
    //     if (a < 0) score += a + both;             //
    //     else score -= a - both;                   //
    // }                                             //
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

/*  ********** free all sig data *********  */
void freeSig_tAllocated(SigStats_t * t) {
    while (t->next != NULL) {
        t = t->next;
        free(t->back);
        t->back = NULL;
    }
}
void freeSig_pAllocated(SigStats_p * p) {
    while (p->next != NULL) {
        p = p->next;
        free(p->back);
        p->back = NULL;
    }
}
/*  ***********************************  */

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
    char * zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
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
        }
    }
}

// collect signifcant stats data
static int callback_sig(void * data, int argc, char ** argv, char ** azColName) {
    int i;
    SigStats_t * tmp_t = ssT_list;
    SigStats_p * tmp_p = ssP_list;

    for (i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "team_wins") == 0) {
            if (tmp_t == NULL) {
                tmp_t = malloc(sizeof(SigStats_t));
                tmp_t->back = NULL;
                tmp_t->next = NULL;
            } else {
                SigStats_t * tmp_back = tmp_t;
                tmp_t->next = malloc(sizeof(SigStats_t));
                tmp_t = tmp_t->next;
                tmp_t->next = NULL;
                tmp_t->back = tmp_back;
            }
            tmp_t->team_wins = atoi(argv[i]);
        } else if (strcmp(azColName[i], "team_loss") == 0) {
            tmp_t->team_loss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "div_leader") == 0) {
            tmp_t->div_leader = atoi(argv[i]);
        } else if (strcmp(azColName[i], "team_pct") == 0) {
            tmp_t->team_pct = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_wins") == 0) {
            if (tmp_p == NULL) {
                tmp_p = malloc(sizeof(SigStats_p));
                tmp_p->back = NULL;
                tmp_p->next = NULL;
            } else {
                SigStats_p * tmp_back = tmp_p;
                tmp_p->next = malloc(sizeof(SigStats_p));
                tmp_p = tmp_p->next;
                tmp_p->next = NULL;
                tmp_p->back = tmp_back;
            }
            tmp_p->wins = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_loss") == 0) {
            tmp_p->loss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_era") == 0) {
            tmp_p->era = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_inningspitched") == 0) {
            tmp_p->innnings = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_strikeouts") == 0) {
            tmp_p->strikeouts = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_baseonballs") == 0) {
            tmp_p->baseonballs = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_hits") == 0) {
            tmp_p->hits = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_homeruns") == 0) {
            tmp_p->homeruns = atoi(argv[i]);
        } else if (strcmp(azColName[i], "pitcher_pct") == 0) {
            tmp_p->pct = atoi(argv[i]);
        } else if (strcmp(azColName[i], "picked_team_mss") == 0) {
            tmp_t->picked_mss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "picked_team_mss_count") == 0) {
            tmp_t->picked_mss_count = atoi(argv[i]);
        } else if (strcmp(azColName[i], "picked_pitcher_mss") == 0) {
            tmp_p->picked_mss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "picked_pitcher_mss_count") == 0) {
            tmp_p->picked_mss_count = atoi(argv[i]);
        } else if (strcmp(azColName[i], "wrong_team_mss") == 0) {
            tmp_t->wrong_mss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "wrong_team_mss_count") == 0) {
            tmp_t->wrong_mss_count = atoi(argv[i]);
        } else if (strcmp(azColName[i], "wrong_pitcher_mss") == 0) {
            tmp_p->wrong_mss = atoi(argv[i]);
        } else if (strcmp(azColName[i], "wrong_pitcher_mss_count") == 0) {
            tmp_p->wrong_mss_count = atoi(argv[i]);
        }
        ssT_list = tmp_t;
        ssP_list = tmp_p;
    }
    return 0;
}

/* collect data from games table and pitchers table */
static int callback(void * data, int argc, char ** argv, char ** azColName) {
    int i;
    
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


int main(int argc, char * argv[]) {
    sqlite3 * db;
    char * zErrMsg = 0;
    int rc;
    char sql[MAX];
    const char * data = "Callback function called";

    rc = sqlite3_open("data/lamp.db", &db);
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


    initPitcherIDs();


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

    /* **** TESTING ****
        printGames();
        printPitchers();
        setPitchers();
       **** TESTING **** */


    for (int i = 0; i < pitchercount; i++) {
        searchForPitcher(&pitcher[i]);
    }


    /* START SIGNIFICANT STAT TABLE SEARCH */
//     snprintf(sql, MAX, "SELECT * FROM [%s]", SIGSTATTABLE);
// 
//     rc = sqlite3_exec(db, sql, callback_sig, (void*)data, &zErrMsg);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "SQL error: %s\n", zErrMsg);
//         sqlite3_free(zErrMsg);
//     }
//     reverseT();
//     reverseP();
    /* END SIGNIFICANT STAT TABLE SEARCH */


    // allocate memory for game data
    Team * bestTeams = malloc(sizeof(Team) * gamecount);
    double * probabilities = malloc(sizeof(double) * gamecount);
    double * differencesInProb = malloc(sizeof(double) * gamecount);


    // begin comparison
    for (int i = 0; i < gamecount; i++) {
        compTeamStats(game[i]);
        compPitcherStats(*game[i].homePitcher, *game[i].awayPitcher);
// 
//         if (score >= 0) {
//             bestTeams[i] = *game[i].home;
//         } else {
//             bestTeams[i] = *game[i].away;
//         }

        // if (score < 0) {
        //     score *= -1;
        // }

        // double prob_home = probability(MAXSCORE, h);
        // double prob_away = probability(MAXSCORE, a);
        // double prob_both = probability(MAXSCORE, both);
        // double prob_diffH = difference_events(prob_home, prob_both);
        // double prob_diffA = difference_events(prob_away, prob_both);
        // printf("prob_home: %.2lf\tprob_away: %.2lf\tprob_both: %.2lf\nprob_diffH: %.2lf\tprob_diffA: %.2lf\n\n", prob_home, prob_away,prob_both, prob_diffH, prob_diffA);

        double prob_home = regProbability(h);
        double prob_away = regProbability(a);
        double prob_both = regProbability(both);
        int homeIsWin = (prob_home > prob_away) ? 1 : 0;

        probabilities[i] = homeIsWin ? prob_home : prob_away;
        differencesInProb[i] = homeIsWin ? difference_events(prob_home, prob_both) : difference_events(prob_away, prob_both);
        bestTeams[i] = homeIsWin ? *game[i].home : *game[i].away;

        score = 0, a = 0, h = 0, both = 0;
    }

    printBestTeams(bestTeams, probabilities, differencesInProb);

    // insert picks into sql server
    char newTable[MAX] = "picks_";
    strcat(newTable, argv[1]);
    createPicksTable(db, newTable);
    insertData(db, newTable, bestTeams, probabilities, gamecount);

    // free all data used
    freeAllAllocated();
    free(bestTeams);
    
    sqlite3_close(db); // close SQLDB

    printf("\npredict.c completed\n");

    return 0;
}