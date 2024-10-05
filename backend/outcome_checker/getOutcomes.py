from path import *
mlb = mlbstatsapi.Mlb()

# get picks from algorithm
def collect_picks(date):
    con, cur = cO.openDB()

    try:
        try:
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

            return pick_abbrs, game_ids

        except sqlite3.OperationalError:
            raise ExceptionsMLB.TableNotExists()
        finally:
            cO.closeDB(con, cur)
    except ExceptionsMLB.TableNotExists as e:
        print(e, file=sys.stderr)
        exit(0)


# get actual outcome of game
def collect_outcomes(game_ids, picks):
    wins = []
    leng = len(game_ids)
    i = -1

    while i < leng:
        i+=1
        print(f'i: {i}\tleng: {leng}')
        # try:
        game = mlb.get_game(game_ids[i])
#         except TypeError:
#             del game_ids[i]
#             del picks[i]
# 
#             i-=1
#             leng-=1
# 
#             continue

        # try:
        home_score = game.livedata.linescore.teams.home.runs
#         except AttributeError:
#             del game_ids[i]
#             del picks[i]
#             i-=1
#             leng-=1
# 
#             continue

        # try:
        away_score = game.livedata.linescore.teams.away.runs
#         except AttributeError:
#             del game_ids[i]
#             del picks[i]
#             i-=1
#             leng-=1
# 
#             continue

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

    table_name = f"outcomes_{date}"

    try:
        cur.execute(f'''CREATE TABLE IF NOT EXISTS `outcomes_{date}` (
            game_id INTEGER PRIMARY KEY,
            team_abbr TEXT NOT NULL,
            correct INTEGER
        )''')

        cur.execute(f"SELECT COUNT(*) FROM `{table_name}`")
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()

        for i in range(len(game_ids)):
            cur.execute(f"INSERT INTO `outcomes_{date}` (game_id, team_abbr, correct) VALUES (?, ?, ?)", (game_ids[i], picks[i], corrects[i]))
            con.commit()
    except (ExceptionsMLB.TableExists) as e:
        print(e, file=sys.stderr)
    finally:
        cO.closeDB(con, cur)

if __name__ == "__main__":
    if len(sys.argv) > 1: 
        date = sys.argv[1]
    else:
        date = d.getYesterday()

    picks, game_ids = collect_picks(date)
    game_ids_list = [i[0] for i in game_ids]
    picks_list = [i[0] for i in picks]

    wins = collect_outcomes(game_ids_list, picks_list)

    query_outcomes(game_ids_list, picks_list, wins, date)