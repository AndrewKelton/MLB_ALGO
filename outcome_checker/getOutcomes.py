import mlbstatsapi
import outcome_checker.checkOutcome as cO
import getDate as d
mlb = mlbstatsapi.Mlb()

# get picks from algorithm
def collect_picks(date):
    con, cur = cO.openDB()
    cur.execute(f'SELECT pick_name FROM `picks_{date}`')

    pick_abbrs = []
    rows = cur.fetchall()
    for row in rows:
        pick_abbrs.append(row)

    cur.execute(f'SELECT game_id FROM `picks_{date}`')

    game_ids = []
    rows = cur.fetchall()
    for row in rows:
        game_ids.append(row)

    cO.closeDB(con, cur)
    return pick_abbrs, game_ids

# get actual outcome of game
def collect_outcomes(game_ids, picks):
    wins = []

    for i in range(len(game_ids)):
        game = mlb.get_game(game_ids[i])
        home_score = game.livedata.linescore.teams.home.runs
        away_score = game.livedata.linescore.teams.away.runs
        home_win = home_score > away_score
        away_win = home_score < away_score

        if home_win and game.gamedata.teams.home.abbreviation == picks[i]:
            wins.append(1)
        elif home_win and game.gamedata.teams.away.abbreviation == picks[i]:
            wins.append(0)
        elif away_win and game.gamedata.teams.away.abbreviation == picks[i]:
            wins.append(1)
        elif away_win and game.gamedata.teams.home.abbreviation == picks[i]:
            wins.append(0)
    return wins

# query outcomes into db
def query_outcomes(game_ids, picks, corrects, date):
    con, cur = cO.openDB()

    cur.execute(f'''CREATE TABLE IF NOT EXISTS `outcomes_{date}` (
        game_id INTEGER PRIMARY KEY,
        team_abbr TEXT NOT NULL,
        correct INTEGER
    )''')

    for i in range(len(game_ids)):
        cur.execute(f"INSERT INTO `outcomes_{date}` (game_id, team_abbr, correct) VALUES (?, ?, ?)", (game_ids[i], picks[i], corrects[i]))
        con.commit()

    cO.closeDB(con, cur)


if __name__ == "__main__":
    date = d.getYesterday()

    picks, game_ids = collect_picks(date)
    game_ids_list = [i[0] for i in game_ids]
    picks_list = [i[0] for i in picks]

    wins = collect_outcomes(game_ids_list, picks_list)

    query_outcomes(game_ids_list, picks_list, wins, date)