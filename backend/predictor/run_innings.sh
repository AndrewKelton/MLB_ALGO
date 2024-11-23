#!/bin/bash

gcc -c innings_instr.c -o innings_instr.o
ar rcs libinnings_instr.a innings_instr.o
gcc innings_predict.c -L. -linnings_instr -o innings_predict

./innings_predict