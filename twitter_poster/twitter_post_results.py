import tweepy
from path import *

mlb = mlbstatsapi.Mlb()

# collect picks stored in twitter picks table
def get_post_picks(date):
    con, cur = cO.openDB()
    game_ids = []
    pick_abbrs = []
    rows = []

    try:
        cur.execute(f"SELECT game_id FROM `twitter_picks_{date}`")
        rows = cur.fetchall()
    except sqlite3.OperationalError as e:
        print(f"Error: {e}")

    for row in rows:
        game_ids.append(row[0])

    try:
        cur.execute(f"SELECT pick_team FROM `twitter_picks_{date}`")
        rows = cur.fetchall()
    except sqlite3.OperationalError as e:
        print(f"Error: {e}")

    for row in rows:
        pick_abbrs.append(row[0])

    cO.closeDB(con, cur)
    
    return game_ids, pick_abbrs

# collect outcomes of each game 
def get_picked_outcomes(date):
    correct = [] 
    game_ids = []
    con, cur = cO.openDB()

    cur.execute(f"SELECT * FROM `outcomes_{date}`")
    results = cur.fetchall()

    for row in results:
        game_ids.append(row[0])
        correct.append(row[2])
    
    cO.closeDB(con, cur)

    return correct, game_ids

# return only results of game ids needed
def get_correct(correct, game_ids, game_ids_all):
    fixed_c = []

    for i in range(len(game_ids)):
        for x in range(len(game_ids_all)):
            if game_ids_all[x] == game_ids[i]:
                fixed_c.append(correct[x])
                continue
    return fixed_c

# return tweet string
def create_tweet(game_ids, correct):
    tweet = f"Results From {d.getYesterday()}\u2757\n\n"
    for i in range(len(correct)):
        game = mlb.get_game(game_ids[i])
        if correct[i] == 0:
            tweet += game.gamedata.teams.away.abbreviation + ' @ ' + game.gamedata.teams.home.abbreviation + ' \u274C\n'
        else:
            tweet += game.gamedata.teams.away.abbreviation + ' @ ' + game.gamedata.teams.home.abbreviation + ' \u2713\n'
    tweet += '\n#mlb #betting #machinelearning'
    return tweet


if __name__ == "__main__":
    date = d.getYesterday()
    game_ids, pick_abbrs = get_post_picks(date)
    correct, game_ids_all = get_picked_outcomes(date)
    tweet = create_tweet(game_ids, get_correct(correct, game_ids, game_ids_all))
    print(tweet)
    

    api_key = ''
    api_secret_key = ''
    access_token = ''
    access_secret_token = ''
    client_id = ''
    client_secret = ''
    bearer_token = ''

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
    post_result = newapi.create_tweet(text=tweet) 
    print(post_result,"\n")
