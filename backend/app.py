from flask import  Flask, jsonify
from flask_cors import CORS
from backend import getDate
from backend.predictor import getGameDay
from backend.outcome_checker import checkOutcome as cO
from backend.twitter_poster import top3picks as t3p
from backend import ExceptionsMLB
from backend import runner
from backend.predictor.innings_predictor import innings_to_db
import sys
import sqlite3

KEY = "secert_key" # Currently unused, but will be used for user APIs, not in this file though

# files to execute if necessary
PREDICTOR_FILES = [
    "python3 predictor/getGameDay.py", 
    "gcc -o p predictor/predict.c -lsqlite3", 
    f"./p {getDate.getTomorrow()}", 
]

OUTCOME_FILES = [
    "python3 outcome_checker/getOutcomes.py", 
    "gcc -o s outcome_checker/sig_stats.c -lsqlite3", 
    f"./s {getDate.getYesterday()}",
]

app=Flask(__name__)
CORS(app)

# Collect gameday data
@app.route('/api/run_games')
def run_games():
    date = getDate.getToday()

    getGameDay.createGamedayTable(date)
    getGameDay.createPitcherTable(date)

    try:
        getGameDay.getGameday(date, getGameDay.TEAMS)
        return True
    except ValueError:
        return False

# Get games for current date
@app.route('/api/games')
def get_games():
    try:
        return jsonify(getGameDay.get_games()), 200
    
    except ExceptionsMLB.TableNotExists as e:
        try:
            getGameDay.createGamedayTable(date=getDate.getToday())
            getGameDay.createPitcherTable(date=getDate.getToday())
            getGameDay.getGameday(date=getDate.getToday())
            predict_games_date(None)
            
            return jsonify(getGameDay.get_games()), 200
        
        except ExceptionsMLB.TableExists:
            print(ExceptionsMLB.ExceptionsMLB.table_exists_msg)
            return jsonify({"error": "No games for day!"})
        
        except AttributeError:
            return jsonify({"error": "System error"}), 404
    
# Get games for certain date
@app.route('/api/games/<date>', methods=['GET'])
def get_games_date(date):
    date = str(date)

    try:
        games = getGameDay.get_games(date)
        return jsonify(getGameDay.get_games(date)), 200
    
    except ExceptionsMLB.TableNotExists:
        try:
            getGameDay.createGamedayTable(date=date)
            getGameDay.createPitcherTable(date=date)
            getGameDay.getGameday(date=date, teams=getGameDay.TEAMS)
            predict_games_date(date)

            return jsonify(getGameDay.get_games(date=date)), 200
        
        except ExceptionsMLB.TableExists:
            print(ExceptionsMLB.ExceptionsMLB.table_exists_msg)
            return jsonify({"error": "No games for day!"}), 304

        except AttributeError:
            return jsonify({"error": "System error"}), 404

# Predict games from api endpoint
@app.route('/api/games/predict')
def predict_games():
    for command in PREDICTOR_FILES:
        stderr = runner.run_command(command)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                return jsonify({"error": "No data for day..."}), 304
            else:
                return jsonify({"error": "server error"}), 404

'''
    Predict games for certain day. Called from '/api/games/<date>'
    endpoint when predictions have not been created for that day.
'''          
def predict_games_date(date):
    for command in PREDICTOR_FILES:
        stderr = runner.run_command(command)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                return jsonify({"error": "No data for day..."}), 304
            else:
                return jsonify({"error": "server error"}), 404

# Get outcomes for yesterday's game from api endpoint
@app.route('/api/game/outcomes') 
def get_outcomes():
    try:
        row_count = ExceptionsMLB.ExceptionsMLB.count_rows('outcomes' , getDate.getYesterday())

        if row_count != 0:
            raise ExceptionsMLB.TableExists()
        else:
            for command in OUTCOME_FILES:
                stderr = runner.run_command(command)

                if stderr:
                    if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                        return jsonify({"error": "No data for day..."}), 304
                    else:
                        return jsonify({"error": "server error"}), 404
                    
    except ExceptionsMLB.TableExists as te:
        return jsonify({"error": "outcomes exist already"})

'''
    Get outcomes for a certain day. Called from another
    endpoint when outcomes have not been created for that day.
'''
def get_outcomes_date(date : str):
    try:
        row_count = ExceptionsMLB.ExceptionsMLB.count_rows('outcomes' ,date)

        if row_count != 0:
            raise ExceptionsMLB.TableExists()
        else:
            for command in OUTCOME_FILES:
                stderr = runner.run_command(command)

                if stderr:
                    if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                        return jsonify({"error": "No data for day..."}), 304
                    else:
                        return jsonify({"error": "server error"}), 404
                    
    except ExceptionsMLB.TableExists as te:
        return jsonify({"error": "outcomes exist already"})

# Gets highest percentage of picked games
def get_top_picks(date : str):
    try:
        row_count = ExceptionsMLB.ExceptionsMLB.count_rows('twitter_picks', date)

        if row_count > 0:
            return jsonify(t3p.get_picked_games(date))
        else:
            raise ExceptionsMLB.TableNotExists()

    except ExceptionsMLB.TableNotExists:
        return jsonify({"error": "No data for day..."}), 304

# Returns NO run 1st innings picks
def get_innings_picks_date(date : str):
    try:
        innings_to_db(date)
        return jsonify(innings_to_db.db_to_innings(date)), 204

    except ExceptionsMLB.TableExists:
        return jsonify({"error": "outcomes exist already"}), 304
    
# TEST
def post_picks(key : str):
    if key is KEY:
        print("key is KEY")


# Run the backend api
if __name__ == '__main__':
    app.run(debug=True)   