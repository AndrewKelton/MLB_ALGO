import sqlite3
import mlbstatsapi
import sys
import checkOutcome as cO
mlb = mlbstatsapi.Mlb()

if __name__ == "__main__":
    date = str(sys.argv[1])
    
    try:
        con, cur = cO.openDB()
        cur.execute(f"SELECT * FROM `picks_{date}`")
        rows = cur.fetchall()

        print("AWAY @ HOME | PICK PERCENT")
        for row in rows:
            game_id, percent, pick = row
            game = mlb.get_game(game_id)

            print(game.gamedata.teams.away.abbreviation + " @ " + game.gamedata.teams.home.abbreviation + "\t" + pick, str(percent) + "%")

    except sqlite3.Error as e:
        print(f"Error occured: {e}")
    finally:
        if con and cur:
            cO.closeDB(con, cur)
        elif con:
            con.close()