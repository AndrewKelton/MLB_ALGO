import sqlite3
import mlbstatsapi
import getDate as d
import ExceptionsMLB
import sys
mlb = mlbstatsapi.Mlb()

if __name__ == "__main__":
    date = d.getTomorrow()
    date = d.getTomorrow()
    
    try:
        con = sqlite3.connect("data/lamp.db")
        cur = con.cursor()
        cur.execute(f"SELECT * FROM `picks_{date}`")
        rows = cur.fetchall()
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()

    except ExceptionsMLB.TableExists as e:
        print(e, file=sys.stderr)
    except sqlite3.Error as e:
        print(e, file=sys.stderr)
        row_count = cur.fetchone()[0]

        if row_count != 0:
            raise ExceptionsMLB.TableExists()

    except ExceptionsMLB.TableExists as e:
        print(e, file=sys.stderr)
    except sqlite3.Error as e:
        print(e, file=sys.stderr)

        print("AWAY @ HOME | PICK PERCENT")


        for row in rows:
            game_id, percent, pick = row
            try:
                game = mlb.get_game(game_id)
            except TypeError:
                continue

            print(game.gamedata.teams.away.abbreviation + " @ " + game.gamedata.teams.home.abbreviation + "\t" + pick, str(percent) + "%")

    except sqlite3.Error as e:
        print(f"Error occured: {e}")
    finally:
        if con and cur:
            cur.close()
            con.close()
        elif con:
            con.close()