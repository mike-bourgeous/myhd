#!/bin/bash
case "$1" in 
	"")
		echo "Specify photos to view on the command line"
		;;
	*)
		modprobe i2c-algo-bit
		insmod driver/tl880.ko
		insmod tl880fb/tl880fb.ko
		
		TLFB="/dev/fb`grep tl880 /proc/fb | cut -d ' ' -f 1`"
		
		echo fbi -T 20 -d $TLFB $@
		fbi -T 20 -d $TLFB $@

		while (ps -A | grep -v grep | grep fbi > /dev/null 2>&1); do sleep 1; done

		sleep 2

		rmmod tl880fb
		rmmod tl880

		;;
esac


