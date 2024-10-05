# import subprocess
from sqlite3 import IntegrityError
from collections import Counter
from path import *


# query data in twitter picks db
def query_top_3(date, id, name):
    con, cur = cO.openDB()
    try:
        cur.execute(f'''CREATE TABLE IF NOT EXISTS `twitter_picks_{date}` (
            game_id INTEGER PRIMARY KEY,
            pick_team TEXT NOT NULL
        )''')

        cur.execute(f"SELECT COUNT(*) FROM `twitter_picks_{date}`")
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()

    except ExceptionsMLB.TableExists as e:
        print(e, file=sys.stderr)
    except sqlite3.Error as e:
        print(e, file=sys.stderr)

    try:
        cur.execute(f"INSERT INTO `twitter_picks_{date}` (game_id, pick_team) VALUES (?,?)", (id, name)) 
        con.commit()
    except IntegrityError:
        print("Values already inserted for " + date)
    finally:
        cO.closeDB(con, cur)

# unused
def top_3(games):
    top3 = {}
    vals = []
    for game in games:
        if len(top3) < 3:
            top3.update(game)
        else:
            cur_vals = game.values()
            for g in top3:
                top_vals = g.values()
                top_key = g.keys()
                if cur_vals[1] > top_vals[1]:
                    top3.pop(top_key)
                    top3.update(game)
    return top3
        
# unused
def keep_3(games_ids):
    if not check_game_ids(games_ids):
        return games_ids
    
    seen = set()
    fixed_list = []
    for i in games_ids:
        if not i in seen:
            fixed_list.append(i)
            seen.add(i)
    return fixed_list

# checker for game ids to check for dupes
def check_game_ids(games_id):
    counts = Counter(games_id)
    for count in counts.values():
        if count > 1:
            return True
    return False
    
# split tuple amongst type of data
def parse_from_sql(games):
    games_id = []
    percents_win = []
    teams_abbr = []

    for game in games:
        print(game)
        game_id, percent_win, team_abbr = game
        games_id.append(game_id)
        percents_win.append(percent_win)
        teams_abbr.append(team_abbr)
    
    return games_id, percents_win, teams_abbr

# collect data of top 3 games (can be > 3 if there are percents that are same)
def get_3_games(best_3):
    table = "picks_" + d.getTomorrow()
    con, cur = cO.openDB()
    query = f"SELECT * FROM `{table}` WHERE percent_win = ?"

    cur.execute(query, (float(best_3[0]),))
    result1 = cur.fetchall()
    if len(best_3) > 1:
        cur.execute(query, (float(best_3[1]),))
        result2 = cur.fetchall()
        if len(best_3) > 2:
            cur.execute(query, (float(best_3[2]),))
            result3 = cur.fetchall()
        else:
            return result1, result2
    else:
        return result1
    cO.closeDB(con, cur)

    return result1, result2, result3

# collect pick data from printGame file
def get_all_p(i):
    date = [d.getTomorrow()]
    result = subprocess.run(['python3', 'printGame.py'] + date, capture_output=True, text=True)
    output = result.stdout.strip()
    res_list = output.split()
    
    res_list = res_list[6:]

    percents = []

    while i < len(res_list):
        percents.append(res_list[i])
        i+=5
   
    return percents

# keep top 3 percents X (don't care) the rest
def get_top_p(percents):
    print(percents)
    try:
        percents_fl = [float(p.strip('%')) for p in percents]
    except ValueError as e:
        return get_top_p(get_all_p(5))
        
    best_3 = []

    for percent in percents_fl:
        if len(best_3) < 3:
            best_3.append(percent)
            best_3.sort(reverse=True)
        else:
            if percent > best_3[-1]:
                best_3[-1] = percent
                best_3.sort(reverse=True)
    
    return best_3

# # # TESTING # # # 
def print_3(best):
    print(best)

def print_all(percents):
    print(percents)
# # # TESTING # # # 


if __name__ == "__main__":
    
    best_3 = get_top_p(get_all_p(4))
    
    results = get_3_games(best_3)

    # partition the results from get_3_games func
    if len(results) == 1:
        result1 = results
    elif len(results) == 2:
        result1, result2 = results
    elif len(results) == 3:
        result1, result2, result3 = results
    
    # collect data from sql

    if len(results) == 1:
        res1 = parse_from_sql(result1)
    elif len(results) == 2:
        res1 = parse_from_sql(result1)
        res2 = parse_from_sql(result2)
    elif len(results) == 3:
        res1 = parse_from_sql(result1)
        res2 = parse_from_sql(result2)
        res3 = parse_from_sql(result3)

    total_games = []
    
    if len(results) >= 1:
        total_games.append(res1)
    if len(results) >= 2:
        total_games.append(res2)
    elif len(results) >= 3:
        total_games.append(res3)
    
    if res1 == res2:
        total_games.remove(res2)
    if res2 == res3:
        total_games.remove(res3)

    all_game_ids = [] # game ids
    all_percent_wins = [] # percent better
    all_team_picks = [] # picked team list
    all_diffin_percents = [] # difference in percentages list

    for pick in total_games:

        game_ids, percent_wins, team_picks = pick # need to add diffin_percents

        for i in range(len(game_ids)):
            all_game_ids.append(game_ids[i])
            all_percent_wins.append(percent_wins[i])
            all_team_picks.append(team_picks[i])

            # diffin_percents.append(diffin_percents[i]) to be added

    # query picks to be tweeted
    for i in range(len(all_game_ids)):
        query_top_3(d.getTomorrow(), all_game_ids[i], all_team_picks[i])