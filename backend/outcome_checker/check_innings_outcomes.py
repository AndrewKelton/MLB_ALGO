from backend import ExceptionsMLB
from backend.outcome_checker import checkOutcome as cO


#

def collect_picks(date):
    con, cur = cO.openDB("backend/data/innings.db")

    try:
        try:
            cur.execute(f'SELECT prediction FROM `innings_{date}`')\

