from backend import ExceptionsMLB
from backend.runner import run_command
import sys

DATA_FILE = 'python3 get_innings_data.py'

if __name__ == '__main__':
    stderr = run_command(DATA_FILE)

    if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
            print("No data for day...")
    else:
        print(f"Error: " + stderr + " in " + DATA_FILE[8:])
        exit(1)
