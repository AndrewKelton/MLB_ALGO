# !/bin/bash

tmr=$(date -v+1d +%F)
yday=$(date -v-1d +%F)
tday=$(date +%F)

# echo -e "Getting yesterday's outcomes \n"
# 
# python3 getOutcomes.py $yday
# if [ $? -eq 0]; then
#     echo "getOutcomes.py finished successfully"
# else 
#     echo "getOutcomes.py failed"
#     exit
# fi
# python3 twitter_post_results.py $yday

# gcc -o s sig_stats.c -lsqlite3
# ./s $yday
# rm s

python3 getOutcomes.py
python3 twitter_post_results.py
gcc -o s sig_stats.c -lsqlite3
./s $yday

python3 getGameDay.py

gcc -o p predict.c -lsqlite3
./p $tday
rm p

python3 twitter_post.py
# 
# python3 getGameDay.py $tmr
# 
# gcc -o p predict.c -lsqlite3
# ./p $tmr
# rm p
# 
# python3 twitter_post.py $tmr