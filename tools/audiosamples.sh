#!/bin/bash

RATES="16000 22050 24000 32000 44100 48000 64000 66000 72000 88200 96000 128000 176400 192000"

echo "This script generates a series of test .wav files at varying sample rates"

if [ "$1" == "" ]; then
	echo "You must specify an audio filename"
	exit 0
fi


for r in $RATES; do 
	echo ${r}Hz
	sox "$1" -r $r $r.wav trim 0 30 rabbit -c2
	sync
done

