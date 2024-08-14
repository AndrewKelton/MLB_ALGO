import sqlite3

con = sqlite3.connect("data/lamp.db")
cur = con.cursor()

name = 'picks_2024-07-07'

cur.execute(f"DROP TABLE IF EXISTS `{'picks_2024-08-14'}`")
con.commit()
cur.execute(f"DROP TABLE IF EXISTS `{'pitchers_2024-08-14'}`")
con.commit()
cur.execute(f"DROP TABLE IF EXISTS `{'games_2024-08-14'}`")
con.commit()

cur.close()
con.close()