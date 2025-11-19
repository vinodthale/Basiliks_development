#!/bin/bash
qcc -Wall -O2 -disable-dimensions circle-droplet.c -o circle-droplet -L$BASILISK/gl -lglutils -lfb_tiny -lm
./circle-droplet 
