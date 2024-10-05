import tweepy
from path import *

keys = ApiKeys()
mlb = mlbstatsapi.Mlb()


# collect top 3 picks from sql
def get_top3_sql(date):
    con, cur = cO.openDB()

    query = f"SELECT * FROM `twitter_picks_{str(date)}`"
    cur.execute(query)
    res = cur.fetchall()
    print(res)

    # only one team picked
    if len(res) == 1:
        id = []
        pick = []
        id_res, pick_res = res[0]

        id.append(id_res)
        pick.append(pick_res)

        return id, pick

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

    game_ids, picks = get_top3_sql(date) # collect query

    # create dictionary of game info
    games = {}

    for i in range(min(3, len(game_ids))):
        games.update(get_game_info(game_ids[i], picks[i]))

    tweet = create_tweet(games) 
        

    # authenticator
    auth = tweepy.OAuth1UserHandler(keys.api_key, keys.api_secret_key)
    auth.set_access_token (
        keys.access_token,
        keys.access_secret_token
    )

    # connect to new api
    newapi = tweepy.Client(
        bearer_token=keys.bearer_token,
        access_token=keys.access_token,
        access_token_secret=keys.access_secret_token,
        consumer_key=keys.api_key,
        consumer_secret=keys.api_secret_key
    )

    # connect to old api
    api = tweepy.API(auth)  

    # tweet the tweet
    print(tweet)
    post_result = newapi.create_tweet(text=tweet) 
    print(post_result,"\n")