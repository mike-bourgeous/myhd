#!/bin/bash
# Extracts firmware from version 1.54 Win9x driver for the MyHD card
# Mike Bourgeous, 2003

let MCUINSN_OFFSET=0x3BA88
let MCUINSN_LENGTH=0x72A*2
let MCUDATA_OFFSET=0x3C8E0
let MCUDATA_LENGTH=0x200*2

let TSDINSN_OFFSET=0x39D9C
let TSDINSN_LENGTH=0x6DD*2
let TSDDATA_OFFSET=0x0
let TSDDATA_LENGTH=0x0

echo Checking md5sum..  if md5sum fails, firmware is probably corrupt
grep Janus.sys README | md5sum --check

echo Extracting MyHD firmware...

echo Extracting MCU firmware from Janus.sys at offset $MCUINSN_OFFSET

dd if=Janus.sys of=mcufirmware.bin bs=1 skip=$MCUINSN_OFFSET count=$MCUINSN_LENGTH

echo Extracting MCU data from Janus.sys at offset $MCUDATA_OFFSET

dd if=Janus.sys of=mcudata.bin bs=1 skip=$MCUDATA_OFFSET count=$MCUDATA_LENGTH

echo Extracting TSD firmware from Janus.sys at offset $TSDINSN_OFFSET

dd if=Janus.sys of=tsdfirmware.bin bs=1 skip=$TSDINSN_OFFSET count=$TSDINSN_LENGTH

#echo Extracting TSD data from Janus.sys at offset $TSDDATA_OFFSET

#dd if=Janus.sys of=tsddata.bin bs=1 skip=$TSDDATA_OFFSET count=$TSDDATA_LENGTH

