#!/bin/bash

./dpc/dpcsync 1 1 0

sleep 8

./iocwritereg 10000 3f

./osdtest
./cursor

sleep 2

./iocwritereg 10000 80

