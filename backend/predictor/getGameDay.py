from backend.predictor.path import *

mlb = mlbstatsapi.Mlb()
TEAMS = ['Arizona Diamondbacks','Atlanta Braves','Baltimore Orioles','Boston Red Sox','Chicago White Sox','Chicago Cubs','Cincinnati Reds','Cleveland Guardians','Colorado Rockies','Detroit Tigers','Houston Astros','Kansas City Royals','Los Angeles Angels','Los Angeles Dodgers','Miami Marlins','Milwaukee Brewers','Minnesota Twins','New York Yankees','New York Mets','Oakland Athletics','Philadelphia Phillies','Pittsburgh Pirates','San Diego Padres','San Francisco Giants','Seattle Mariners','St. Louis Cardinals','Tampa Bay Rays','Texas Rangers','Toronto Blue Jays','Washington Nationals']

# create gameday table in DB
def createGamedayTable(date):
    con, cur = cO.openDB(None)
    table_name = f"games_{date}"

    try:
        cur.execute(f'''CREATE TABLE IF NOT EXISTS `games_{date}` (
            game_id INTEGER PRIMARY KEY,
            home_team TEXT NOT NULL,
            away_team TEXT NOT NULL,
            home_wins INTEGER,
            away_wins INTEGER,
            home_loss INTEGER,
            away_loss INTEGER,
            home_div_leader INTEGER,
            away_div_leader INTEGER,
            same_div INTEGER,
            home_pitcher_id INTEGER,
            away_pitcher_id INTEGER
        )''')

        cur.execute(f"SELECT COUNT(*) FROM `{table_name}`")
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()
    except ExceptionsMLB.TableExists as e:
        print(e, file=sys.stderr)
    finally:
        cO.closeDB(con, cur)

# create pitcher table in DB
def createPitcherTable(date):
    con, cur = cO.openDB(None)

    table_name = f"pitchers_{date}"

    try:
        cur.execute(f'''CREATE TABLE IF NOT EXISTS `pitchers_{date}` (
            pitcher_id INTEGER PRIMARY KEY,
            wins INTEGER,
            losses INTEGER,
            era REAL,
            inningspitched REAL,
            strikeouts INTEGER,
            baseonballs REAL,
            hits INTEGER,
            homeruns INTEGER
        )''')

        cur.execute(f"SELECT COUNT(*) FROM `{table_name}`")
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()
    except ExceptionsMLB.TableExists as e:
        print(e, file=sys.stderr)
    finally:
        cO.closeDB(con, cur)

# insert game
def queryGame(date, game_id, home, away, hw, aw, hl, al, hdv, adv, sd, h_pitcherID, a_pitcherID):
    con, cur = cO.openDB(None)

    cur.execute(f"INSERT INTO `games_{date}` (game_id, home_team, away_team, home_wins, away_wins, home_loss, away_loss, home_div_leader, away_div_leader, same_div, home_pitcher_id, away_pitcher_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (game_id, home, away, hw, aw, hl, al, hdv, adv, sd, h_pitcherID, a_pitcherID))
    con.commit()
    # print("Games inserted into DB")

    cO.closeDB(con, cur)

# insert pitcher
def queryPitcher(date, pitcherID, p_stats):
    con, cur = cO.openDB(None)

    cur.execute(f"INSERT INTO `pitchers_{date}` (pitcher_id, wins, losses, era, inningspitched, strikeouts, baseonballs, hits, homeruns) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", (pitcherID, p_stats[0], p_stats[1], p_stats[2], p_stats[3], p_stats[4], p_stats[5], p_stats[6], p_stats[7]))
    con.commit()

    cO.closeDB(con, cur)

# get pitcher stats from api
def get_pitcher_stats(player_id):
    stats = ['season']
    groups = ['pitching']
    pitcher_stats = mlb.get_player_stats(player_id, stats, groups)

    p_stats = [0,0,0,0,0,0,0,0]

    if pitcher_stats and 'pitching' in pitcher_stats and 'season' in pitcher_stats['pitching']:
        season_stat = pitcher_stats['pitching']['season']
        i = 0
        for split in season_stat.splits:
            i = 0
            # print(f"Season: {split.season}")
            # print(f"Wins: {split.stat.wins}")
            p_stats[i] = split.stat.wins
            # print(f"Losses: {split.stat.losses}")
            i+=1
            p_stats[i] = split.stat.losses
            # print(f"ERA: {split.stat.era}")
            i+=1
            p_stats[i] = (split.stat.era)
            # print(f"Innings Pitched: {split.stat.inningspitched}")
            i+=1
            p_stats[i] = (split.stat.inningspitched)
            # print(f"Strikeouts: {split.stat.strikeouts}")
            i+=1
            p_stats[i] = (split.stat.strikeouts)
            # print(f"Walks: {split.stat.baseonballs}")
            i+=1
            p_stats[i] = (split.stat.baseonballs)
            # print(f"Hits: {split.stat.hits}")
            i+=1
            p_stats[i] = (split.stat.hits)
            # print(f"Home Runs: {split.stat.homeruns}")
            i+=1
            p_stats[i] = (split.stat.homeruns)
            # print("-" * 20)
    else:
        print(f"No stats available for player ID {player_id}")

    return p_stats

# get whole gameday stats 
def getGameday(date, teams):
    team_id = []
    game_id = []

    for team in teams:
        team_id.append(mlb.get_team_id(team))

    games = mlb.get_scheduled_games_by_date(date=date)
    game_id = [game.gamepk for game in games]
    
    pitcher_default = 0

    for id in game_id:
        try:
            game = mlb.get_game(id)
        except TypeError: # TypeError means game is already in progress
            continue

        home = game.gamedata.teams.home
        away = game.gamedata.teams.away

        home_team = home.abbreviation
        away_team = away.abbreviation

        # Wins/Losses
        home_recW = home.record.wins
        away_recW = away.record.wins
        home_recL = home.record.losses
        away_recL = away.record.losses
        homeWP = home.record.winningpercentage
        awayWP = away.record.winningpercentage

        # pitcher stats
        home_pitcher = 0
        away_pitcher = 0
        try:
            home_pitcher = game.gamedata.probablepitchers.home.id
        except AttributeError:
            home_pitcher = pitcher_default
            pitcher_default += 1
        
        try:
            away_pitcher = game.gamedata.probablepitchers.away.id
        except AttributeError:
            away_pitcher = pitcher_default
            pitcher_default += 1
        
        h_pitcher_stats = get_pitcher_stats(home_pitcher)
        a_pitcher_stats = get_pitcher_stats(away_pitcher)
        
        # Check divisions/leaders
        homed = home.division.id
        awayd =  away.division.id
        isSameD = (homed == awayd)
        homedLeader = home.record.divisionleader
        awaydLeader = away.record.divisionleader

        # Convert to int value
        sameD = 0
        homeD = 0
        awayD = 0
        if isSameD:
            sameD = 1
        if homedLeader:
            homeD = 1
        if awaydLeader:
            awayD = 1
        
        # print(home_team + away_team)

        # Query info
        queryGame(date, id, home_team, away_team, home_recW, away_recW, home_recL, away_recL, homeD, awayD, sameD, home_pitcher, away_pitcher)
        queryPitcher(date, home_pitcher, h_pitcher_stats)
        queryPitcher(date, away_pitcher, a_pitcher_stats)

# returns list of dictionaries of each game for day
def get_games(date=d.getToday()):
    con, cur = cO.openDB("data/lamp.db")
    
    rows = []
    try:
        print("Date in function: " + date)
        cur.execute(f"SELECT game_id, home_team, away_team FROM `games_{date}`")# `{date}`
        rows = cur.fetchall()
        games = []
        picks = get_predictions(con, cur, date)
        # Iterate through the fetched rows and create a dictionary for each game
        i = 0
        for row in rows:
            game_dict = {
                "game_id": row[0],         # game_id
                "home_team": row[1],       # home_team
                "away_team": row[2],        # away_team
                "pick_name": picks[i]
            }
            i+=1
            games.append(game_dict)

        print(games)

        cO.closeDB(con, cur)
        return games

    except sqlite3.DatabaseError as e:
        raise ExceptionsMLB.TableNotExists
    
def get_predictions(con, cur, date):
    # con, cur = cO.openDB("data/lamp.db")

    rows = []
    try:
        cur.execute(f"SELECT pick_name FROM `picks_{date}`")# `games_{str(d.getToday())}`")
        rows = cur.fetchall()
        picks = []

        # Iterate through the fetched rows and create a dictionary for each game
        for row in rows:
            # game_dict = {
            #     "game_id": row[0],         # game_id
            #     "home_team": row[1],       # home_team
            #     "away_team": row[2]        # away_team
            # }
            picks.append(row[0])

        # cO.closeDB(con, cur)
        return picks

    except sqlite3.DatabaseError as e:
        raise ExceptionsMLB.TableNotExists
    

# return game info for specified game id
def get_game_info(game_id):
    game = mlb.get_game(game_id)

    time = game.gamedata.datetime.time
    location = game.gamedata.venue.location
    home = game.gamedata.teams.home.abbreviation
    away = game.gamedata.teams.away.abbreviation

    return {'game_id': game_id, 'time': time, 'location': location, 'teams': away + '@' + home}


if __name__ == "__main__":
    get_games()
#     date = d.getTomorrow()
# 
#     createGamedayTable(date)
#     createPitcherTable(date)
# 
#     print("Getting games for " + date)
# 
#     TEAMS = ['Arizona Diamondbacks','Atlanta Braves','Baltimore Orioles','Boston Red Sox','Chicago White Sox','Chicago Cubs','Cincinnati Reds','Cleveland Guardians','Colorado Rockies','Detroit Tigers','Houston Astros','Kansas City Royals','Los Angeles Angels','Los Angeles Dodgers','Miami Marlins','Milwaukee Brewers','Minnesota Twins','New York Yankees','New York Mets','Oakland Athletics','Philadelphia Phillies','Pittsburgh Pirates','San Diego Padres','San Francisco Giants','Seattle Mariners','St. Louis Cardinals','Tampa Bay Rays','Texas Rangers','Toronto Blue Jays','Washington Nationals']
# 
#     getGameday(date, TEAMS)