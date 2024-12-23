import sqlite3
import pandas as pd
import sys
import os

# FILE = f'innings_{gd.getTomorrow()}'
# FILE = f'innings_2024-08-21'

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

    df = pd.read_csv('backend/predictor/innings_predictor/tmp.csv')
    print(df)

    rows = df.values.tolist()
    
    for row in rows:
        # row[2] = str(row[2])
        # print(row)
        cur.execute(f"INSERT INTO `{FILE}` (home_abr, away_abr, prediction, result, res_b, res_db) VALUES (?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2], row[3], row[4], row[5]))
        print(row)
        con.commit()

    cur.close(), con.close()

    os.remove("tmp.csv")
