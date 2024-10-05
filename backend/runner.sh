# !/bin/bash

if [ $# -eq 0 ]; then
    python3 runner.py
else 
    python3 runner.py $1
fi

# python3 runner.py
rm -r __pycache__