# !/bin/bash

tmr=$(date -v+1d +%F)
yday=$(date -v-1d +%F)
tday=$(date +%F)

DB="data/lamp.db"
TABLE="games_$tmr"
SQL_QUERY="SELECT name FROM sqlite_master WHERE type='table' AND name='$TABLE';"
TABLE_EXISTS=$(sqlite3 "$DB" "$SQL_QUERY")

if [ -z "$TABLE_EXISTS" ]; then
    python3 getGameDay.py $tmr

    TABLE="picks_$tday"
    SQL_QUERY="SELECT name FROM sqlite_master WHERE type='table' AND name='$TABLE';"
    TABLE_EXISTS=$(sqlite3 "$DB" "$SQL_QUERY")

    if [ -z "$TABLE_EXISTS" ]; then
        gcc -o p predict.c -lsqlite3
        ./p $tmr

        TABLE="outcomes_$yday"
        SQL_QUERY="SELECT name FROM sqlite_master WHERE type='table' AND name='$TABLE';"
        TABLE_EXISTS=$(sqlite3 "$DB" "$SQL_QUERY")

        if [ -z "$TABLE_EXISTS" ]; then
            TABLE="picks_$yday"
            SQL_QUERY="SELECT name FROM sqlite_master WHERE type='table' AND name='$TABLE';"
            TABLE_EXISTS=$(sqlite3 "$DB" "$SQL_QUERY")
            python3 twitter_post.py $tmr

            # git checkout -b my_working_branch
            # git add data
            # git commit
            # git checkout main
            # git merge my_working_branch
            # git push

            if [ -z "$TABLE_EXISTS" ]; then
                echo "NO DATA FROM YESTERDAY"
            else
                echo "Checking yesterday's outcomes"
                python3 getOutcomes.py $yday
                
                gcc -o s sig_stats.c -lsqlite3
                ./s $yday
            fi
        else
            python3 printGame.py $tmr
        fi
    else 
        python3 printGame.py $tmr
    fi
else 
    python3 printGame.py $tmr
fi
