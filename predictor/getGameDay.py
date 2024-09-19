from path import *

mlb = mlbstatsapi.Mlb()

# create gameday table in DB
def createGamedayTable(date):
    con, cur = cO.openDB()
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
    con, cur = cO.openDB()

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
    con, cur = cO.openDB()

    cur.execute(f"INSERT INTO `games_{date}` (game_id, home_team, away_team, home_wins, away_wins, home_loss, away_loss, home_div_leader, away_div_leader, same_div, home_pitcher_id, away_pitcher_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (game_id, home, away, hw, aw, hl, al, hdv, adv, sd, h_pitcherID, a_pitcherID))
    con.commit()
    # print("Games inserted into DB")

    cO.closeDB(con, cur)

# insert pitcher
def queryPitcher(date, pitcherID, p_stats):
    con, cur = cO.openDB()

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
        except TypeError:
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

if __name__ == "__main__":
    date = d.getTomorrow()

    createGamedayTable(date)
    createPitcherTable(date)

    print("Getting games for " + date)

    TEAMS = ['Arizona Diamondbacks','Atlanta Braves','Baltimore Orioles','Boston Red Sox','Chicago White Sox','Chicago Cubs','Cincinnati Reds','Cleveland Guardians','Colorado Rockies','Detroit Tigers','Houston Astros','Kansas City Royals','Los Angeles Angels','Los Angeles Dodgers','Miami Marlins','Milwaukee Brewers','Minnesota Twins','New York Yankees','New York Mets','Oakland Athletics','Philadelphia Phillies','Pittsburgh Pirates','San Diego Padres','San Francisco Giants','Seattle Mariners','St. Louis Cardinals','Tampa Bay Rays','Texas Rangers','Toronto Blue Jays','Washington Nationals']

    getGameday(date, TEAMS)