#!/bin/bash

./iocwritereg 4 8
./iocwritereg 10008 60
./dpc/dpcsync 0 0 0

