#!/bin/bash
# This script randomizes the DPC2 registers

echo Randomizing DPC2 registers
./iocwritereg 10180 0 > /dev/null
for f in `seq 0 1023`; do 
	./iocwritereg 10184 `printf "%04x%04x" $RANDOM $RANDOM` > /dev/null
done
	
