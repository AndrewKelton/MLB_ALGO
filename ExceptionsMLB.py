# module holding specific exceptions

class ExceptionsMLB(Exception):
    """Base for other exceptions"""
    table_exists_msg = "Table Exists"
    pass

class TableExists(ExceptionsMLB):
    """Raised when table already exists"""
    def __init__(self, message=ExceptionsMLB.table_exists_msg):
        self.message = message
        super().__init__(self.message)