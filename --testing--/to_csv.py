# convert sql data to csv file

import csv
import sys
import os
import outcome_checker.checkOutcome as cO
import getDate as d
import multi_regression as mr

# collect data from sql db
def get_from_sql(date):
    con, cur = cO.openDB()

    table = "games_" + date
    query = f"SELECT * FROM `{table}`"

    cur.execute(query)
    results = cur.fetchall()

    cO.closeDB(con, cur)
    return results

def parse_data():
    data = get_from_sql(d.getTomorrow())

    for i in range(len(data)):
        game = data[i]
        home_team = []
        away_team = []
        
        x = 0
        while x < len(game) - 6:
            x+=1
            if x == 1 or x == 2: continue
            home_team.append(game[x])
            x+=1
            away_team.append(game[x])
        
        home_team_tup = tuple(home_team)
        away_team_tup = tuple(away_team)

        # print([home_team_tup, away_team_tup])
        convert_csv([home_team_tup, away_team_tup])
        mr.multi()
        os.remove("data.csv")

# convert data to csv
def convert_csv(teams):
    with open('data.csv', 'w') as out:
        csv_out = csv.writer(out)
        csv_out.writerow(['wins', 'loss'])
        csv_out.writerows(teams)

if __name__ == '__main__':
    parse_data()