from backend.outcome_checker import checkOutcome as cO
from backend import ExceptionsMLB
import sqlite3
import mlbstatsapi

mlb = mlbstatsapi.Mlb()

# return list of home team's ids
def get_innings_abr(date : str):
    table_name = f'''innings_{date}'''

    con, cur = cO.openDB('backend/data/inning.db')

    try:
        cur.execute(f'''SELECT home_abr FROM `{table_name}`''')
        home_abrs = []
        rows = cur.fetchall()

        for row in rows:
            home_abrs.append(row[0])
        cO.closeDB(con, cur)

        return home_abrs
    
    except sqlite3.DatabaseError:
        raise ExceptionsMLB.TableNotExists()

# return list of ids from date
def get_game_ids(date : str):
    return cO.getGameIDs(date)

# remove uneeded ids
def remove_ids(home_abrs : list, game_ids : list):
    false_checks = 0

    for a in home_abrs:
        curr_id = 0

        for id in game_ids:
            curr_id = id

            game = mlb.get_game(id)

            if a == game.gamedata.teams.home.abbreviation:
                false_checks = 0
                break
            else:
                false_checks += 1

        if false_checks > 0:
            false_checks = 0
            game_ids.remove(curr_id)
    
    return game_ids

# link games and return dictionary
def link_id_inning(home_abrs : list, game_ids : list):
    linked_games = []

    for h in home_abrs:
        for game in game_ids:
            if h == mlb.get_game(id).gamedata.teams.home.abbreviation:
                linked_games.append({h : game})
                game_ids.remove(game)
    
    return linked_games