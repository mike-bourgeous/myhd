#!/bin/bash

./ioctl 40085618 1000 # ntsc
sleep 0.2
./ioctl 4004760f 3f0  # ch 3
sleep 0.2
./ioctl 5646 0        # print status
sleep 0.2
./ioctl 64c0 0        # decoder dump
sleep 0.2
dmesg | tail -n 100

