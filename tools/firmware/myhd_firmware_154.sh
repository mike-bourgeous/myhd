#!/bin/bash
# Extracts firmware from version 1.54 Win9x driver for the MyHD card
# Mike Bourgeous, 2003

let MCUINSN_OFFSET=0x36500
let MCUINSN_LENGTH=0x72A*2
let MCUDATA_OFFSET=0x37358
let MCUDATA_LENGTH=0x200*2

let TSDINSN_OFFSET=0x34928
let TSDINSN_LENGTH=0x701*2
let TSDDATA_OFFSET=0x3DF74
let TSDDATA_LENGTH=0x5D6*2

echo Checking md5sum..  if md5sum fails, firmware is probably corrupt
grep Mdp100.sys README | md5sum --check

echo Extracting MyHD firmware...

echo Extracting MCU firmware from Mdp100.sys at offset $MCUINSN_OFFSET

dd if=Mdp100.sys of=mcufirmware.bin bs=1 skip=$MCUINSN_OFFSET count=$MCUINSN_LENGTH

echo Extracting MCU data from Mdp100.sys at offset $MCUDATA_OFFSET

dd if=Mdp100.sys of=mcudata.bin bs=1 skip=$MCUDATA_OFFSET count=$MCUDATA_LENGTH

echo Extracting TSD firmware from Mdp100.sys at offset $TSDINSN_OFFSET

dd if=Mdp100.sys of=tsdfirmware.bin bs=1 skip=$TSDINSN_OFFSET count=$TSDINSN_LENGTH

#echo Extracting TSD data from Mdp100.sys at offset $TSDDATA_OFFSET

#dd if=Mdp100.sys of=tsddata.bin bs=1 skip=$TSDDATA_OFFSET count=$TSDDATA_LENGTH

