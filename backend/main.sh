# !/bin/bash

tmr=$(date -v+1d +%F)
yday=$(date -v-1d +%F)
tday=$(date +%F)

set -e

directories=("outcome_checker", "twitter_poster", "predictor")
files=("python3 outcome_checker/getOutcomes.py", "gcc -o s outcome_checker/sig_stats.c -lsqlite3", "./s $yday", "python3 twitter_poster/twitter_post_results.py", "python3 predictor/getGameDay.py", "gcc -o p predictor/predict.c -lsqlite3", "./p $tmr", "python3 twitter_poster/top3picks.py", "python3 twitter_poster/twitter_post.py")

for file in "${files[@]}"
do
    echo "Executing: $file"

    eval $file
    # status=$?

    # if [ $status -ne 0]; then
    #     echo "Error: $file failed with exit code $status. Exiting..."
    #     exit 1
    # fi
done

echo "Algorithm completed successfully"