#!/bin/bash
# Extracts firmware from version 1.54 Win9x driver for the MyHD card
# Mike Bourgeous, 2003

let MCUINSN_OFFSET=0x3F230
let MCUINSN_LENGTH=0x72A*2
let MCUDATA_OFFSET=0x40088
let MCUDATA_LENGTH=0x200*2

let TSDINSN_OFFSET=0x3D568
let TSDINSN_LENGTH=0x701*2
let TSDDATA_OFFSET=0x0
let TSDDATA_LENGTH=0x0

echo Checking md5sum..  if md5sum fails, firmware is probably corrupt
grep Mdp100_2K.sys README | md5sum --check

echo Extracting MyHD firmware...

echo Extracting MCU firmware from Mdp100_2K.sys at offset $MCUINSN_OFFSET

dd if=Mdp100_2K.sys of=mcufirmware.bin bs=1 skip=$MCUINSN_OFFSET count=$MCUINSN_LENGTH

echo Extracting MCU data from Mdp100_2K.sys at offset $MCUDATA_OFFSET

dd if=Mdp100_2K.sys of=mcudata.bin bs=1 skip=$MCUDATA_OFFSET count=$MCUDATA_LENGTH

echo Extracting TSD firmware from Mdp100_2K.sys at offset $TSDINSN_OFFSET

dd if=Mdp100_2K.sys of=tsdfirmware.bin bs=1 skip=$TSDINSN_OFFSET count=$TSDINSN_LENGTH

# echo Extracting TSD data from Mdp100_2K.sys at offset $TSDDATA_OFFSET

# dd if=Mdp100_2K.sys of=tsddata.bin bs=1 skip=$TSDDATA_OFFSET count=$TSDDATA_LENGTH

