import sqlite3
import pandas as pd
# from backend.outcome_checker import checkOutcome as cO
# from backend import getDate as gd


# FILE = f'innings_{gd.getTomorrow()}'
FILE = f'innings_2024-08-21'


if __name__ == '__main__':
    con = sqlite3.connect("backend/data/innings.db")
    cur = con.cursor()
    
    cur.execute(f'''CREATE TABLE IF NOT EXISTS `{FILE}` (
        home_abr TEXT NOT NULL,
        away_abr TEXT NOT NULL,
        prediction TEXT NOT NULL,
        result DOUBLE,
        res_b DOUBLE,
        res_db DOUBLE
    )''')

    df = pd.read_csv('backend/predictor/innings_predictor/tmp.csv')
    print(df)
    
    cur.close(), con.close()
