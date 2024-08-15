import tweepy
import subprocess
import sys
import ast

def get_picks():
    result = subprocess.run(['python3', 'top3picks.py'] + sys.argv[1:], capture_output=True, text=True)
    output = result.stdout.strip()
    out_dict = {}
    try:
        out_dict = ast.literal_eval(output)
        return out_dict
    except (ValueError, SyntaxError) as e:
        print(f"failure to create dictionary {e}")

def create_tweet(games):
    tweet = "Tomorrows's Top Picks\u2757\n\n"
    for val in games.values():
        tweet += val[0] + " @ " + val[1] + " PICK: " + val[2] + " @ " + val[4] + "\n"
    tweet += '\n#mlb #betting #machinelearning'
    return tweet 
        
if __name__ == "__main__":
    api_key = '''You can't see'''
    api_secret_key = '''You can't see'''
    access_token = '''You can't see'''
    access_secret_token = '''You can't see'''
    client_id = '''You can't see'''
    client_secret = '''You can't see'''
    bearer_token = '''You can't see'''

    auth = tweepy.OAuth1UserHandler(api_key, api_secret_key)
    auth.set_access_token (
        access_token,
        access_secret_token
    )

    newapi = tweepy.Client(
        bearer_token=bearer_token,
        access_token=access_token,
        access_token_secret=access_secret_token,
        consumer_key=api_key,
        consumer_secret=api_secret_key
    )

    api = tweepy.API(auth)

    tweet = create_tweet(get_picks())

    post_result = newapi.create_tweet(text=tweet)

    print(post_result)

