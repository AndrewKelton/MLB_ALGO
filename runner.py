import os
import sys
import subprocess
import getDate as date
import ExceptionsMLB
import sqlite3
import getDate as d
import mlbstatsapi
import ExceptionsMLB


def run_command(command):
    
    # Execute the command and capture the output
    try:
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        return e.stderr.decode('utf-8')

    return result.stderr.decode('utf-8')

def clean(o):
    try:
        # Get the current working directory
        current_dir = os.path.dirname(os.path.abspath(__file__))
        
        # Construct the full file path
        file_path = os.path.join(current_dir, o)
        
        if os.path.exists(file_path):
            os.remove(file_path)
    except Exception as e:
        print(f"An error occurred while trying to remove the file: {e}")

if __name__ == "__main__":
    print("\n---- " + '\033[1m' + "MLB ALGO" + '\033[0m' + " ----\n")

    outcome_files = [
        "python3 outcome_checker/getOutcomes.py", 
        "gcc -o s outcome_checker/sig_stats.c -lsqlite3", 
        f"./s {d.getYesterday()}", 
        "python3 twitter_poster/twitter_post_results.py"
    ]
    
    predictor_files = [
        "python3 predictor/getGameDay.py", 
        "gcc -o p predictor/predict.c -lsqlite3", 
        f"./p {d.getTomorrow()}", 
        "python3 twitter_poster/top3picks.py", 
        "python3 twitter_poster/twitter_post.py"
    ]

    o_files = [
        "p",
        "s"
    ]

    directories = [
        "outcome_checker",
        "predictor",
        "twitter_poster"
    ]
    
    # execute outcome module
    for file in outcome_files:
        stderr = run_command(file)

        if stderr:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr:
                # print(stderr + "\n\nMoving to predictions module")
                break
            else:
                print(f"Error in {file}: " + stderr)
                exit(1)

    # execute prediction module
    for file in predictor_files:
        stderr = run_command(file)

        if stderr and file not in predictor_files[0]:
            if ExceptionsMLB.ExceptionsMLB.table_exists_msg in stderr:
                # print(stderr + "\n\nPredictions have previously been created")
                break
            else:
                print(f"Error in {file}: " + stderr)
                exit(1)
    
    for o in o_files:
        clean(o)
    
    print("\nMLB ALGO completed successfully...\n")