# Main shell for project
''' 
            ...INFORMATION...

Calls all modules for project based on flags 
raised from user. If no flags are raised by user, 
program runs all modules consecutively.

Flags are raised by passing them as command line arguments.

Usage: 
    python3 runner.py -p -outcome  # gets outcomes and prints outcomes to terminal

'''

'''
        !!! IMPORTANT !!!

THIS IS ONLY USED FOR COMMAND LINE USAGE!!
app.js DOES NOT USE THIS AND NEVER WILL!!
THERE IS NO API SUPPORT FOR THIS FILE!!

'''

import os
import sys
import shutil
import subprocess
from backend import getDate as d
from backend import ExceptionsMLB
import sqlite3
import mlbstatsapi
from enum import Flag, auto


# Flag class for FLAGS
class Flags(Flag):
    P = auto()           # 1
    OUTCOME = auto()     # 2
    PREDICT = auto()     # 4
    TWEET_0 = auto()     # 8
    TWEET_1 = auto()     # 16
    ALL = P | OUTCOME | PREDICT | TWEET_0 | TWEET_1

FLAGS_DI = {
    Flags.P: "-p",
    Flags.OUTCOME: "-outcome",
    Flags.PREDICT: "-predict",
    Flags.TWEET_0: "-tweet 0",
    Flags.TWEET_1: "-tweet 1",
    Flags.ALL: "-all"
}


# outcome checker module files
OUTCOME_FILES = [
    "python3 outcome_checker/getOutcomes.py", 
    "gcc -o s outcome_checker/sig_stats.c -lsqlite3", 
    f"./s {d.getYesterday()}", 
    "python3 twitter_poster/twitter_post_results.py"
]

# predictor module files
PREDICTOR_FILES = [
    "python3 predictor/getGameDay.py", 
    "gcc -o p predictor/predict.c -lsqlite3", 
    f"./p {d.getTomorrow()}", 
    "python3 twitter_poster/top3picks.py", 
    "python3 twitter_poster/twitter_post.py"
]

# diretories storing python modules
DIRECTORIES = [
    "outcome_checker",
    "predictor",
    "twitter_poster",
]

# outputted binary files
O_FILES = [
    "p",
    "s"
]


# run the command inputted using array
def run_command(command):
    
    # Execute the command and capture the OUTCOME
    try:
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        return e.stderr.decode('utf-8')

    return result.stderr.decode('utf-8')


# remove binary files from directory... clean the folder up
def clean(o):

    try:
        # Get the current working directory
        current_dir = os.path.dirname(os.path.abspath(__file__))
        
        # Construct the full file path
        file_path = os.path.join(current_dir, o)

        if os.path.exists(file_path):

            if os.path.isdir(file_path):
                shutil.rmtree(file_path)
            else:
                os.remove(file_path)

    except Exception as e:
        print(f"An error occurred while trying to remove the file: {e}")


# check for flags
def parse_flags(flag_strings : list):
    parsed_flags = Flags(0)

    try:
        flag_strings.remove('runner.py')
    except ValueError:
        print("Error parsing flags...", file=sys.stderr)
        exit(1)

    print(flag_strings)

    try:
        for flag_str in flag_strings:
            if flag_str == "tweet":
                
                try:
                    if flag_strings[flag_strings.index(flag_str) + 1] == None or int(flag_strings[flag_strings.index(flag_str) + 1]) < 0 or int(flag_strings[flag_strings.index(flag_str) + 1]) > 1:
                        raise ExceptionsMLB.InvalidFlag(flag_str)
                    else:
                        flag_str += " " + flag_strings.pop(flag_strings.index(flag_str) + 1)
                except ExceptionsMLB.InvalidFlag as e:
                    print(e, file=sys.stderr)
                    print_usage()
            
            if flag_str in FLAGS_DI.values():
                for key, value in FLAGS_DI.items():
                    if value == flag_str:
                        parsed_flags |= key  # Combine flags using bitwise OR
            else:
                raise ExceptionsMLB.InvalidFlag(flag_str)
            
    except ExceptionsMLB.InvalidFlag as e:
        print(e, file=sys.stderr)
        print_usage()
        
    return parsed_flags

# id flags
def parse_flag(flag):
    print(flag.value)
    return flag
        

# print usage of flags
def print_usage():
    print("\nUsage:\n-p (print)\n-outcome (outcome_checker)\n-predict (predictor)\n-tweet 0 (twitter_post_results)\n-tweet 1 (twitter_poster)\n-all (normal run)\n", file=sys.stderr)
    sys.exit(1)


# run outcome module
def outcome_runner():
    for file in OUTCOME_FILES:
        stderr = run_command(file)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
                print("No data for day...")
                break
            else:
                print(f"Error in {file}: " + stderr)
                exit(1)

# run twitter_post_results.py
def twitter_runner_0():
    stderr = run_command("python3 twitter_poster/twitter_post_results.py")

    if stderr:
        if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
            print("No data for day...")
        else:
            print(f"Error in twitter_poster/twitter_post_results.py: " + stderr)
            exit(1)

# twitter_post.py
def twitter_runner_1():
    stderr = run_command("python3 twitter_poster/twitter_post.py")
    
    if stderr:
        if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr or ExceptionsMLB.ExceptionsMLB.table_not_exists_msg in stderr:
            print("No data for day...")
        else:
            print(f"Error in twitter_poster/twitter_post.py: " + stderr)
            exit(1)

# run predictor module
def predictor_runner():
    for file in PREDICTOR_FILES:
        stderr = run_command(file)

        if stderr and file not in PREDICTOR_FILES[0]:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr:
                print("Data already inputted...")
                break
            else:
                print(f"Error in {file}: " + stderr)
                exit(1)


# run all modules/files
def all():
    outcome_runner()
    predictor_runner()


# parse enums in Flags and run modules from flags
def run(flags):
    fs_val = flags.value

    if flags == Flags.ALL:
        all()
    elif flags == Flags.OUTCOME:
        outcome_runner()
    elif flags == Flags.PREDICT:
        predictor_runner()
    elif flags == Flags.P:
        run_command("python3 printGame.py")
    elif flags == Flags.TWEET_0:
        twitter_runner_0()
    elif flags == Flags.TWEET_1:
        twitter_runner_1()
        
    elif fs_val == (Flags.P | Flags.OUTCOME).value:
        outcome_runner()
        # create outcome print function
    elif fs_val == (Flags.P | Flags.PREDICT).value:
        predictor_runner()
        run_command("python3 printGame.py")
    elif fs_val == (Flags.PREDICT | Flags.TWEET_1).value:
        predictor_runner()
        twitter_runner_1()
    elif fs_val == (Flags.OUTCOME | Flags.PREDICT).value:
        outcome_runner()
        predictor_runner()

    elif fs_val == (Flags.P | Flags.OUTCOME | Flags.PREDICT).value:
        outcome_runner()
        predictor_runner()
        run_command("python3 printGame.py")
    elif fs_val == (Flags.OUTCOME | Flags.PREDICT | Flags.TWEET_0).value:
        outcome_runner()
        predictor_runner()
        twitter_runner_0()
    elif fs_val == (Flags.OUTCOME | Flags.PREDICT | Flags.TWEET_1).value:
        outcome_runner()
        predictor_runner()
        twitter_runner_1()
        
    elif fs_val == (Flags.P | Flags.OUTCOME | Flags.PREDICT | Flags.TWEET_0).value:
        outcome_runner()
        predictor_runner()
        twitter_runner_0()
        run_command("python3 printGame.py")
    elif fs_val == (Flags.P | Flags.OUTCOME | Flags.PREDICT | Flags.TWEET_1).value:
        outcome_runner()
        predictor_runner()
        twitter_runner_1()
        run_command("python3 printGame.py")

    else:
        print("Flag combination has not been created yet!")
        print_usage()
        sys.exit(1)


if __name__ == "__main__":
    print("\n---- " + '\033[1m' + "MLB ALGO" + '\033[0m' + " ----\n")

    user_flags = Flags(0)

    # Parse user flags from command line
    if len(sys.argv) > 1:
        flags = parse_flags(sys.argv)
        user_flags = parse_flag(flags)
    else:
        user_flags = Flags.ALL

    run(user_flags)
    
    # clean project directories
    for o in O_FILES and DIRECTORIES:
        if o in DIRECTORIES:
            o += "/__pycache__"
        clean(o)

    print("\nMLB ALGO completed successfully...\n")