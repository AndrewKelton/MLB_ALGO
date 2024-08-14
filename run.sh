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

    TABLE="picks_$tmr"
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
            if [ -z "$TABLE_EXISTS" ]; then
                echo "NO DATA FROM YESTERDAY"
            else
                echo "Checking yesterday's outcomes"
                python3 checkOutcome.py $yday
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
