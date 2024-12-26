# import sys, os
# # 
# current_dir = os.path.dirname(os.path.abspath(__file__))
# sys.path.append(os.path.abspath(os.path.join(current_dir, '..')))
# from runner import *
# 
# import sqlite3
# import getDate as d
# import mlbstatsapi
# import ExceptionsMLB
# from outcome_checker import checkOutcome as cO

import sys, os

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.abspath(os.path.join(current_dir, '..')))
from runner import *
from outcome_checker import checkOutcome as cO

# could just use variables but I like keeping
# all of the same data in an object, keeps it
# more organized
class ApiKeys():
    api_key = '''YOU CAN'T SEE THIS :)'''
    api_secret_key = '''YOU CAN'T SEE THIS :)'''
    access_token = '''YOU CAN'T SEE THIS :)'''
    access_secret_token = '''YOU CAN'T SEE THIS :)'''
    client_id = '''YOU CAN'T SEE THIS :)'''
    client_secret = '''YOU CAN'T SEE THIS :)'''
    bearer_token = '''YOU CAN'T SEE THIS :)'''
    pass
