import tweepy
import subprocess
import outcome_checker.checkOutcome as cO
import mlbstatsapi
import getDate as d
mlb = mlbstatsapi.Mlb()

# collect top 3 picks from sql
def get_top3_sql(date):
    con, cur = cO.openDB()

    query = f"SELECT * FROM `twitter_picks_{str(date)}`"
    cur.execute(query)
    res = cur.fetchall()

    ids = []
    picks = []
    for game in res:
        ids.append(game[0])
        picks.append(game[1])
    
    cO.closeDB(con, cur)

    return ids, picks

# get game info
def get_game_info(game, pick):
    g = mlb.get_game(game)
    home_team = g.gamedata.teams.home.abbreviation
    away_team = g.gamedata.teams.away.abbreviation
    time = g.gamedata.datetime.time + " " + g.gamedata.datetime.ampm + " " + g.gamedata.venue.timezone.tz

    return {game: [away_team, home_team, pick, time]}

# create picks tweet
def create_tweet(games):
    tweet = f"Top Picks {d.getTomorrow()}\u2757\n\n"
   
    for val in games.values():
        tweet += val[0] + " @ " + val[1] + " PICK: " + val[2] + " @ " + val[3] + "\n"
    tweet += '\n#mlb #betting #machinelearning'
    return tweet 


if __name__ == "__main__":
    # query top picks for posting
    date = d.getTomorrow()
    dlist = [date]
    try:
        subprocess.run(['python3', 'top3picks.py'] + dlist, capture_output=False, text=True)
    except Exception as e:
        print(f"Error: {e}")

    game_ids, picks = get_top3_sql(date) # collect query

    # create dictionary of game info
    games = {}
    for i in range(3):
        games.update(get_game_info(game_ids[i], picks[i]))

    tweet = create_tweet(games) 
        
    api_key = 
    api_secret_key = 
    access_token = 
    access_secret_token = 
    client_id = 
    client_secret = 
    bearer_token = 

    # authenticator
    auth = tweepy.OAuth1UserHandler(api_key, api_secret_key)
    auth.set_access_token (
        access_token,
        access_secret_token
    )

    # connect to new api
    newapi = tweepy.Client(
        bearer_token=bearer_token,
        access_token=access_token,
        access_token_secret=access_secret_token,
        consumer_key=api_key,
        consumer_secret=api_secret_key
    )

    # connect to old api
    api = tweepy.API(auth)  

    # tweet the tweet
    print(tweet)
    post_result = newapi.create_tweet(text=tweet) 
    print(post_result,"\n")
