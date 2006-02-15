#!/bin/bash

./dpc/dpcsync 1 1 0

sleep 5

./iocwritereg 10000 3f

./osdtest
./cursor

./osdimage cloud.raw

./iocwritereg 10000 80
./dpc/videoout 0

