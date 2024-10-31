# module holding specific MLB exceptions 
from backend.outcome_checker import checkOutcome as cO
import sqlite3

class ExceptionsMLB(Exception):
    """Base for other exceptions"""
    table_exists_msg="Table Exists"
    table_not_exists_msg="Table Not Exists"
    game_not_played="Game Did Not Play"
    invalid_flag="invalid flag"

    @staticmethod
    def count_rows(table : str, date : str) -> int:
        con, cur = cO.openDB()

        try:
            cur.execute(f"SELECT COUNT(*) FROM `{table}_{date}`")
            return cur.fetchone()[0]
        except sqlite3.OperationalError:
            raise TableNotExists()
        finally:
            cO.closeDB(con, cur)

    pass

class TableExists(ExceptionsMLB):
    """Raised when table already exists"""
    def __init__(self, message=ExceptionsMLB.table_exists_msg):
        self.message = message
        super().__init__(self.message)

class TableNotExists(ExceptionsMLB):
    """Raised when `sqlite3.OperationalError` is raised"""
    def __init__(self, message=ExceptionsMLB.table_not_exists_msg):
        self.message=message
        super().__init__(self.message)
        
class GameNotPlayed(ExceptionsMLB):
    """
    Raised when AttributeError: 'dict' object has no attribute
    'runs' occurs when getting outcomes from games
    """
    def __init__(self, message=ExceptionsMLB.game_not_played):
        self.message=message
        super().__init__(self.message)
    
class InvalidFlag(ExceptionsMLB):
    """Raised when caller uses invalid flag when calling runner.py"""
    def __init__(self, flag, message=ExceptionsMLB.invalid_flag):
        self.flag = flag  
        self.message = f"{message}... {flag}"
        super().__init__(self.message)
        