from flask import  Flask, jsonify
from flask_cors import CORS
from backend import getDate
from backend.predictor import getGameDay
from backend import ExceptionsMLB 


app=Flask(__name__)
CORS(app)

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

@app.route('/api/games')
def get_games():
    try:
        games = getGameDay.get_games()
        print(games)

        return jsonify(getGameDay.get_games())
    
    except ExceptionsMLB.TableNotExists as e:
        print(ExceptionsMLB.ExceptionsMLB.table_not_exists_msg)

        return jsonify({"error": "No games for day!"})
    
@app.route('/api/games/<int:game_id>', methods=['GET'])
def get_game_details(game_id):
    game = next((g for g in getGameDay.get_games() if g['game_id'] == game_id), None)

    if game:
        return jsonify(getGameDay.get_game_info(game_id=game_id))
    else:
        return jsonify({"error": "Game not found"}), 404


if __name__ == '__main__':
    app.run(debug=True)   