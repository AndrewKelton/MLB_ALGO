import mlbstatsapi
import sqlite3
import getDate as d

mlb = mlbstatsapi.Mlb()

# Open the DB for interaction
def openDB():
    con = sqlite3.connect("../data/lamp.db")
    cur = con.cursor()
    return con, cur

# Close the DB interaction
def closeDB(con, cur):
    cur.close()
    con.close()

# create outcome table in DB
def createOutcomeTable():
    con, cur = openDB()

    cur.execute(f'''CREATE TABLE IF NOT EXISTS `outcomes_{str(d.getYesterday())}` (
        game_id INTEGER PRIMARY KEY,
        team_abr TEXT NOT NULL,
        correct INTEGER
    )''')

    closeDB(con, cur)

# insert outcome into DB
def queryOutcome(game_id, team_id, correct):
    con, cur = openDB()

    cur.execute(f"INSERT INTO `outcomes_{str(d.getYesterday())}` (game_id, team_abr, correct) VALUES (?, ?, ?)", (game_id, team_id, correct))
    con.commit()
    print("Outcomes have been inserted")

    closeDB(con, cur)

# get the picked team abbreviation
def getPickAbr():
    con, cur = openDB()

    cur.execute(f"SELECT pick_name FROM `picks_{str(d.getYesterday())}`")

    picks_abr = []

    rows = cur.fetchall()
    for row in rows:
        picks_abr.append(row[0])
    closeDB(con, cur)

    return picks_abr

# get game IDs from DB... returns game_ids as list
def getGameIDs():
    con, cur = openDB()

    cur.execute(f"SELECT game_id FROM `games_{str(d.getYesterday())}`")

    game_ids = []

    rows = cur.fetchall()
    for row in rows:
        game_ids.append(row[0])
    closeDB(con, cur)

    return game_ids

# get outcome from api ~~return as list of 1s and 0s~~
def getFinalScores():
    game_ids = getGameIDs()
    picks_abr = getPickAbr()

    correct = []

    i = 0
    for id in game_ids:
        game = mlb.get_game(id)

        home_score = game.livedata.linescore.teams.home.runs
        away_score = game.livedata.linescore.teams.home.runs

        if int(home_score) > int(away_score) and picks_abr[i] == game.gamedata.teams.home.abbreviation:
            correct.append(1)
        elif int(home_score) > int(away_score) and picks_abr[i] != game.gamedata.teams.home.abbreviation:
            correct.append(0)
        elif int(home_score) < int(away_score) and picks_abr[i] == game.gamedata.teams.away.abbreviation:
            correct.append(1)
        elif int(home_score) < int(away_score) and picks_abr[i] != game.gamedata.teams.away.abbreviation:
            correct.append(0)
        else:
            correct.append(0)

        queryOutcome(id, picks_abr[i], correct[i])
        i += 1
        # decisionTree(id, picks_abr[i], correct[i])


# get team stats and pitcher stats functions to implement
def getTeamStats(game_id):
    con, cur = openDB()

    cur.execute(f"SELECT * FROM `game_{str(d.getYesterday())}` WHERE id = ?", (game_id,))
    row = cur.fetchone()

    closeDB(con, cur)
    if row:
        return row
    print("NO ROW FOUND")

    return []

''' 

HERE

'''

# Decision Tree Algorithm (TO BE IMPLEMENTED PERHAPS IN C INSTEAD)
def decisionTree(game_id, pick, correct):
    total = 0
    if correct == 0:
        return 0
    else:
        total += 1
        team_stats = getTeamStats(game_id)
        print(team_stats)
            
# main
if __name__ == "__main__":
    createOutcomeTable()
    getFinalScores()