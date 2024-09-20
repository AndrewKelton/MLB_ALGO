# module holding specific exceptions

class ExceptionsMLB(Exception):
    """Base for other exceptions"""
    table_exists_msg = "Table Exists"
    table_not_exists_msg = "Table Not Exists"
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
