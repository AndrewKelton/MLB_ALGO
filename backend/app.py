from flask import  Flask, jsonify
from flask_cors import CORS
from backend import getDate
from backend.predictor import getGameDay
from backend.outcome_checker import checkOutcome as cO
from backend import ExceptionsMLB 
from backend import runner
import sqlite3

PREDICTOR_FILES = [
    "python3 predictor/getGameDay.py", 
    "gcc -o p predictor/predict.c -lsqlite3", 
    f"./p {getDate.getTomorrow()}", 
]

app=Flask(__name__)
CORS(app)

# Collect gameday data
@app.route('/api/run_games')
def run_games():
    date = getDate.getToday()

    getGameDay.createGamedayTable(date)
    getGameDay.createPitcherTable(date)

    TEAMS = ['Arizona Diamondbacks','Atlanta Braves','Baltimore Orioles','Boston Red Sox','Chicago White Sox','Chicago Cubs','Cincinnati Reds','Cleveland Guardians','Colorado Rockies','Detroit Tigers','Houston Astros','Kansas City Royals','Los Angeles Angels','Los Angeles Dodgers','Miami Marlins','Milwaukee Brewers','Minnesota Twins','New York Yankees','New York Mets','Oakland Athletics','Philadelphia Phillies','Pittsburgh Pirates','San Diego Padres','San Francisco Giants','Seattle Mariners','St. Louis Cardinals','Tampa Bay Rays','Texas Rangers','Toronto Blue Jays','Washington Nationals']

    try:
        getGameDay.getGameday(date, TEAMS)
        return True
    except ValueError:
        return False

# Get games for current date
@app.route('/api/games')
def get_games():
    try:
        games = getGameDay.get_games()
        return jsonify(getGameDay.get_games()), 200
    
    except ExceptionsMLB.TableNotExists as e:
        try:
            getGameDay.createGamedayTable(date=getDate.getToday())
            getGameDay.createPitcherTable(date=getDate.getToday())
            getGameDay.getGameday(date=getDate.getToday())
            predict_games_date(None)
            
            return jsonify(getGameDay.get_games()), 200
        
        except ExceptionsMLB.TableExists as te:
            print(ExceptionsMLB.ExceptionsMLB.table_exists_msg)
            return jsonify({"error": "No games for day!"})
        
        except AttributeError as ae:
            return jsonify({"error": "System error"}), 404
    
# Get games for certain date
@app.route('/api/games/<date>', methods=['GET'])
def get_games_date(date):
    print("date: " + date)
    date = str(date)
    try:
        games = getGameDay.get_games(date)
        return jsonify(getGameDay.get_games(date)), 200
    
    except ExceptionsMLB.TableNotExists as e:
        try:
            getGameDay.createGamedayTable(date=date)
            getGameDay.createPitcherTable(date=date)
            getGameDay.getGameday(date=date, teams=getGameDay.TEAMS)
            predict_games_date(date)

            return jsonify(getGameDay.get_games(date=date)), 200
        
        except ExceptionsMLB.TableExists as te:
            print(ExceptionsMLB.ExceptionsMLB.table_exists_msg)
            return jsonify({"error": "No games for day!"}), 304

        except AttributeError as ae:
            return jsonify({"error": "System error"}), 404
    
@app.route('/api/game/predict')
def predict_games():
    for command in PREDICTOR_FILES:
        stderr = runner.run_command(command)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                return jsonify({"error": "No data for day..."}), 304
            else:
                return jsonify({"error": "server error"}), 404
            
def predict_games_date(date):
    for command in PREDICTOR_FILES:
        stderr = runner.run_command(command)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                return jsonify({"error": "No data for day..."}), 304
            else:
                return jsonify({"error": "server error"}), 404
            
# @app.route('/api/games/<int:game_id>', methods=['GET'])
# def get_game_details(game_id):
#     game = next((g for g in getGameDay.get_games() if g['game_id'] == game_id), None)
# 
#     if game:
#         return jsonify(getGameDay.get_game_info(game_id=game_id))
#     else:
#         return jsonify({"error": "Game not found"}), 404

# @app.route('/api/games/predictions')
# def get_all_predictions():
#     try:
#         predictions = cO.getPickAbr('2024-10-01') # getDate.getToday()
#     except sqlite3.DatabaseError:
#         return jsonify({"error": "Games not found"}), 404
#     
#     try:
#         pred_ids = cO.getGameIDs('2024-10-01') # getDate.getToday()
#     except sqlite3.DatabaseError:
#         return jsonify({"error": "Games not found"}), 404
# 
#     pred_dict = {}
#     for i in range(len(predictions)):
#         pred_dict.update(pred_ids[i], predictions[i])
# 
#     return jsonify(pred_dict)

# Run the app
if __name__ == '__main__':
    app.run(debug=True)   