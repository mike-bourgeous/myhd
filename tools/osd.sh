#!/bin/bash

./dpc/dpcsync 1 1 0
./dpc/videoout 1

sleep 3

./iocwritereg 10000 24 

for f in `seq 0 255`; do ./osdtest $f 0 0 255 0 0 `expr 255 - $f` 255 ; done

./cursor

sleep 1

./iocwritereg 10000 80
./dpc/videoout 0

echo "Run dpc/videoout 1 and dpc/dpcsync 1 1 0 to reenable TL880 video"


