# !/bin/bash

day=1
month=8
max=31

for (( m=8; m <= 10; m+++ ));
do
    if [ m == 1 ]; then
        max=30
    else 
        max=31
    fi

    for (( i=1; i<=$max; i++ ));
    do
        if [ i < 10 && m < 10]; then
            ./innings_predict 2024-0$m-0$i
        else if [ i < 10 ]; then
            ./innings_predict 2024-$m-0$i
        else if [ m < 10]; then
            ./innings_predict 2024-0$m-$i
        else
            ./innings_predict 2024-$m-$i
        fi
    done
done