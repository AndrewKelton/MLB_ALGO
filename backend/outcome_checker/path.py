# import sys, os
# 
# current_dir = os.path.dirname(os.path.abspath(__file__))
# sys.path.append(os.path.abspath(os.path.join(current_dir, '..')))
# 
# from runner import *
# import mlbstatsapi
# import sqlite3

import sys, os

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.abspath(os.path.join(current_dir, '..')))
from runner import *
from outcome_checker import checkOutcome as cO