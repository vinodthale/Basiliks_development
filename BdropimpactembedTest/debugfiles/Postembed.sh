#!/bin/bash
ulimit -c unlimited
qcc -Wall -O2 -disable-dimensions Bdropimpactembed.c -o Bdropimpactembed -lm
./Bdropimpactembed
