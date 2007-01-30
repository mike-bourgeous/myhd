#!/bin/bash

./ioctl 40085618 1000 # ntsc
./ioctl 4004760f 3f0  # ch 3
./ioctl 5646 0        # print status
./ioctl 64c0 0        # decoder dump
dmesg | tail -n 100

