import sqlite3

con = sqlite3.connect('innings.db')
con.commit()
con.close()