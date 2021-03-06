**Note: this project will not be resumed.**

My TL880-based card, a MyHD MDP-110, was stolen and thus I have no hardware to
use for development.  Additionally, modern hardware is fast enough to make
standalone MPEG-2 decoder cards completely unnecessary.

## MyHD driver and utility package

https://github.com/mike-bourgeous/myhd/
https://mike-bourgeous.github.io/myhd/
http://myhd.sourceforge.net/

Copyright 2003-2007, Mike Bourgeous (some 2003, Mark Lehrer; see driver/README)
Licensed under the GPL.  See file headers for additional copyright notices.

#### Project layout

- **doc/**
  - These files describe the known features of the chip
- **driver/**
  - Kernel module (no video capture yet - simply allows non-root to access
    the card and enables I2C)
- **tools/**
  - Use these to "play" with the card and figure out how stuff works.
- **tl880fb/**
  - Preliminary framebuffer driver.

#### Description

There are step-by-step instructions on compiling the driver and running tests at
https://mike-bourgeous.github.io/myhd/howtohelp.php.

For more information, please view the mailing list archives.  Also read all the
README files in this driver distribution (find . -name README).  Reading source
code is also good.

Remember that this driver has no video capture capability.  The kernel module
uses three devices per TL880 card in /dev/tl880 to allow all users to mmap the
memory regions of the TL880, or use the ioctl interface to control the card.
The tools are then used to upload the firmware, set external sync, turn on color
bars, display a mouse cursor, and tweak the OSD.  That's all that the driver can
do so far.  There is also I2C bus support.  For the latest known bugs, see the
web site and mailing list.

To test the I2C bus(es), you will need the lm_sensors package.  Load the
i2c-dev, i2c-algo-bit, and tl880.ko kernel modules, then run i2cdetect for each
bus displayed in the kernel message log (each card has a different number of
buses).  If you have any output beside all XX's, note the address and bus
number, and run i2cdump \[bus\] \[address\].  For example, the NXT2000 on the MyHD
has address 0x14 on bus 1, so i2cdetect 1 shows a response from address 0xa, and
i2cdump 1 0xa shows the registers of the NXT2000.


#### Still needed:

- Someone with PCI DMA experience.
- Someone who can figure out how to use WinDBG to peer into the Windows drivers.
- Lots and lots of time.
