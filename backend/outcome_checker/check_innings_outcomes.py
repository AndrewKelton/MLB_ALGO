from backend import ExceptionsMLB
from backend.outcome_checker import checkOutcome as cO
from backend.outcome_checker import getOutcomes as gO
from backend import getDate as d
import sys
import sqlite3
import mlbstatsapi

mlb = mlbstatsapi.Mlb()

# get picks from algorithm
def collect_picks(date):
    con, cur = cO.openDB("backend/data/innings.db")

    try:
        try:
            cur.execute(f'SELECT prediction FROM `innings_{date}`')

            under_picks = []
            rows = cur.fetchall()
            for row in rows:
                under_picks.append(row)

            return under_picks

        
        except sqlite3.OperationalError:
            raise ExceptionsMLB.TableNotExists()
        finally:
            cO.closeDB(con, cur)
    except ExceptionsMLB.TableNotExists as e:
        print(e, file=sys.stderr)
        exit(0)

def collect_outcomes(game_ids, picks):
    wins = []
    leng = len(game_ids)
    i = -1

    while i < leng:
        i+=1

        game = mlb.get_game(game_ids[i])

        score = game.livedata.linescore.innings

        if score > 0 and picks[i] == 0:
            wins.append(0)
        elif score > 0 and picks[i] > 0:
            wins.append(1)
        elif score <= 0 and picks[i] == 0:
            wins.append(1)
        elif score <= 0 and picks[i] > 0:
            wins.append(0)
    
    return wins

def query_outcomes(game_ids, picks, corrects, date):
    gO.query_outcomes(game_ids, picks, corrects, date, "innings_outcomes")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        date = sys.argv[1]
    else:
        date = d.getYesterday()
    
    picks, game_ids = collect_picks(date)
    game_ids_list = [i[0] for i in game_ids]
    picks_list = [i[0] for i in picks]

    wins = collect_outcomes(game_ids, picks_list)

    query_outcomes(game_ids_list, picks_list, wins, date)
