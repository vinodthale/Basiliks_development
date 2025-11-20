#!/bin/bash
ulimit -c unlimited
qcc -Wall -O0 -g -disable-dimensions -DTRASH=1 Bdropimpactembed.c -o Bdropimpactembed -lm
./Bdropimpactembed


