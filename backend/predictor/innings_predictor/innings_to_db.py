from backend import ExceptionsMLB
import link_inning_id as lid
import sqlite3
import pandas as pd
import sys
import os

def innings_to_db(date : str):
    FILE = f'innings_{date}'

    con = sqlite3.connect("backend/data/innings.db")
    cur = con.cursor()
    
    cur.execute(f'''CREATE TABLE IF NOT EXISTS `{FILE}` (
        home_abr TEXT NOT NULL,
        away_abr TEXT NOT NULL,
        prediction BOOLEAN,
        result DOUBLE,
        res_b DOUBLE,
        res_db DOUBLE
    )''')

    row_count = ExceptionsMLB.ExceptionsMLB.count_rows("innings_", date)
    if row_count != 0:
        raise ExceptionsMLB.TableExists()

    df = pd.read_csv('backend/predictor/innings_predictor/tmp.csv')

    rows = df.values.tolist()
    
    for row in rows:
        cur.execute(f"INSERT INTO `{FILE}` (home_abr, away_abr, prediction, result, res_b, res_db) VALUES (?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2], row[3], row[4], row[5]))
        con.commit()

    cur.close(), con.close()

    os.remove("tmp.csv")

# returns format of innings predictions, caller checks if table exists... else good luck
def db_to_innings(date : str):
    DB = f'innings_{date}'

    con = sqlite3.connect("backend/data/innings.db")
    cur = con.cursor()

    try:
        cur.execute(f"SELECT home_abr, away_abr, prediction FROM `{DB}`")
        rows = cur.fetchall()
        games = []
        
        for row in rows:
            game_dict = {   
                "home": row[0],         # home abbreviation
                "away": row[1],         # away abbreviation
                "prediction": row[2]    # prediction of score
            }
            games.append(game_dict)

        cur.close(), con.close()

        return games

    except sqlite3.DatabaseError as e:
        raise ExceptionsMLB.TableNotExists()
    
    finally:
        if con and cur:
            cur.close(), con.close()

if __name__ == '__main__':

    date = sys.argv[1]
    FILE = f'innings_{date}'

    con = sqlite3.connect("backend/data/innings.db")
    cur = con.cursor()
    
    cur.execute(f'''CREATE TABLE IF NOT EXISTS `{FILE}` (
        home_abr TEXT NOT NULL,
        away_abr TEXT NOT NULL,
        prediction BOOLEAN,
        result DOUBLE,
        res_b DOUBLE,
        res_db DOUBLE
    )''')

    row_count = ExceptionsMLB.ExceptionsMLB.count_rows("innings_", date)
    if row_count != 0:
        raise ExceptionsMLB.TableExists()

    df = pd.read_csv('backend/predictor/innings_predictor/tmp.csv')
    print(df)

    rows = df.values.tolist()

    linked = lid.link_id_inning(lid.get_innings_abr(date), lid.remove_ids(lid.get_innings_abr(date), lid.get_game_ids(date)))
    
    for row in rows:
        # row[2] = str(row[2])
        # print(row)
        cur.execute(f"INSERT INTO `{FILE}` (home_abr, away_abr, prediction, result, res_b, res_db) VALUES (?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2], row[3], row[4], row[5]))
        print(row)
        con.commit()

    cur.close(), con.close()

    os.remove("tmp.csv")
