import sqlite3
# date = sys.argv[1]

con = sqlite3.connect("data/lamp.db")
cur = con.cursor()

# name = 'outcomes_'+date
# name1 = 'picks_'+date
# name2 = 'pitchers_'+date
# name3 = 'games_'+date
# name4 = 'significant_stats'


# cur.execute(f"DROP TABLE IF EXISTS `{name4}`")
# con.commit()
cur.execute(f"DROP TABLE IF EXISTS `outcomes_2024-09-12`")
con.commit()
# cur.execute(f"DROP TABLE IF EXISTS `pitchers_2024-09-13`")
# con.commit()
# # 
# # cur.execute(f"DROP TABLE IF EXISTS `{name1}`")
# # con.commit()
# cur.execute(f"DROP TABLE IF EXISTS `{name2}`")
# con.commit()
# cur.execute(f"DROP TABLE IF EXISTS `{name3}`")
con.commit()

cur.close()
con.close()