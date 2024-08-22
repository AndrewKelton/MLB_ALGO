import tweepy
import subprocess
import sys
import ast

# get picks from picks_....
def get_picks():
    result = subprocess.run(['python3', 'top3picks.py'] + sys.argv[1:], capture_output=True, text=True)
    output = result.stdout.strip()
    out_dict = {}
    
    try:
        out_dict = ast.literal_eval(output)
        return out_dict
    except (ValueError, SyntaxError) as e:
        print(f"failure to create dictionary {e}")

# create picks tweet
def create_tweet(games):
    tweet = f"Top Picks {sys.argv[1]}\u2757\n\n"
    for val in games.values():
        tweet += val[0] + " @ " + val[1] + " PICK: " + val[2] + " @ " + val[4] + "\n"
    tweet += '\n#mlb #betting #machinelearning'
    return tweet 


if __name__ == "__main__":
    api_key = '9PpkkRCCWsuPpBmJLiBqmy0AM'
    api_secret_key = 'cdIj6b37biYOeMGJZ3AmvdSyWtE06EbAb67wwzoiftg6u1L3eo'
    access_token = '1681527848614371328-zHlMpTnsg8VyiRpB8YrssuE6vbQOT3'
    access_secret_token = 'nFiD0etYvODX1xqNXr1WwMDLoYdI9XgULXy8bFfROTigr'
    client_id = 'dGtDZXp0MjVodDBFb0hQTTk2dmU6MTpjaQ'
    client_secret = '0VwVjWmw-oz0JdqDSkdLfuBuK5Kc1KN1u-EuMBKRrh_W3MpUfQ'
    bearer_token = 'AAAAAAAAAAAAAAAAAAAAADppvQEAAAAABclQ6nuEDLooLQatIu4QLqMOxcA%3D9aZUECZIOhG2fWjuPsjKzulJvJAXtXCoKWFMuV822ghYP6bOUW'

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
    tweet = create_tweet(get_picks()) 
    post_result = newapi.create_tweet(text=tweet) 