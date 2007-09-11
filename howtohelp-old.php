<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("", "");
?>
<center><h3>How to Help TL880 Driver Development</h3></center>
<p>Right now there are three things people can do to help development.  The first
is writing e-mail to the company that makes your card to let them know about
this driver, and encourage them to help driver development in any way they can.
The second is by running tests on your TL880-based card to allow us to get a
survey of the various hardware out there and see what needs to be done to the
driver to get the broadest support in the shortest time.  The third is running
the <a href="/tl880spy.php">TL880Spy tool</a> in Windows that allows "playing" with the
card's registers.  The TL880Spy tool is based on the BtSpy tool from the 
<a href="http://btwincap.sourceforge.net/">BtWincap</a> open source bttv driver
for Windows.  The tool is used to view and modify registers, for the purpose of
determining the function of the registers.

<p>The two tests currently available for running are the <a href="#i2ctest">I2C bus test</a> and the
<a href="#colorbartest">color bars sync test</a>.  Neither test would have the
ability to damage your hardware; in the worst case, you might need to unplug the
video passthrough of the HDTV card and plug your monitor directly into your
video card.  In the command lines given below, # indicates commands to be run
as root, and $ indicates commands to be run as either root or a normal user.  
The $/# symbols should not be typed.  These tests can be run <!--from the
<a href="/testcd.php">bootable test disc</a>, or -->from an installed Linux
distribution with the TL880 driver compiled.

<a name="i2ctest"></a><h4>I2C Bus Test</h4>
<p>The I2C bus is the interface used by the TL880 chip to communicate to the
other chips on an HDTV tuner card.  Each TL880 card seems to use a different I2C
communication method.  There are several such methods implemented in the driver,
at least one of which should show at least one I2C device on any TL880 card.
Different cards will show a different number of buses.  You will need the
kernel source for your kernel to compile the kernel module.  You will need the
lm_sensors package for the i2c* tools.  Your kernel must have been compiled with
support for I2C devices in /dev and the I2C bit-shift algorithm (also called
"I2C bit-banging interfaces").  Your results will not be identical to those
below.  They are given as examples only.
<br>
<table width="80%" align="center" border="0" cellspacing="15" cellpadding="0">
	<tr>
		<td><tt>$ lynx http://myhd.sourceforge.net/download.php</tt></td>
		<td><i>Download the current driver</i></td>
	</tr>
	<tr>
		<td><tt>$ tar -jxvf myhd-<i>version</i>.tar.bz2</tt></td>
		<td><i>Extract the driver tarball</i></td>
	</tr>
	<tr>
		<td><tt>$ cd myhd-<i>version</i></tt></td>
		<td><i>Change to the driver source directory</i></td>
	</tr>
	<tr>
		<td><tt>$ make</tt></td>
		<td><i>Compile the kernel module (requires kernel source) and tools</i></td>
	</tr>
	<!--
	<tr>
		<td><tt># modprobe i2c-dev; modprobe i2c-algo-bit</tt></td>
		<td><i>Load the kernel modules required by the driver</i></td>
	</tr>
	-->
	<tr>
		<td><tt># dmesg -c</tt></td>
		<td><i>Clear the kernel message log</i></td>
	</tr>
	<tr>
		<td><tt># insmod driver/tl880.ko</tt></td>
		<td><i>Load the kernel module into the running kernel</i></td>
	</tr>
	<tr>
		<td><tt># driver/tl880_mkdev</tt></td>
		<td><i>Create /dev/tl880/* entries for each card in the
		system</i></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Note: the <b><span style="background-color:#b0c0ff">i2c
			bus numbers</span></b> and 
			<b><span style="background-color:#b0ffc0">i2c device
			addresses</span></b> below (i.e. the parameteris given
			to the i2cdetect and i2cdump tools) will be different
			if there are other I2C devices present, such as an
			lm_sensors chip or a TV tuner card, or if your card has
			a different supporting chipset.  The correct bus
			numbers are output to the kernel message log when the
			driver is loaded.  The correct addresses are output by
			the i2cdetect tool.
		</td>
	</tr>
	<tr>
		<td><tt># dmesg</tt></td>
		<td><i>Display the kernel messages from the driver</i></td>
	</tr>
	<tr>
		<td colspan="2"><tt>
tl880: device found, irq 5<br>
tl880: initializing card number 0<br>
tl880: mem: 0xdc000000(0xe5d4e000), reg: 0xe2000000(0xe9d4f000), unk: 0xe2100000(0xe9e50000)<br>
tl880: Found MIT Inc. MyHD MDP110B<br>
tl880: set vip 0<br>
tl880: set vip 1<br>
i2c-dev.o: Registered 'tl880 0-1' as minor 0<br>
i2c-core.o: adapter tl880 0-1 registered as adapter
<b><span style="background-color:#b0c0ff">0</span></b>.<br>
i2c-dev.o: Registered 'tl880 0-2' as minor 1<br>
i2c-core.o: adapter tl880 0-2 registered as adapter
<b><span style="background-color:#b0c0ff">1</span></b>.<br>
i2c-dev.o: Registered 'tl880 0-3' as minor 2<br>
i2c-core.o: adapter tl880 0-3 registered as adapter
<b><span style="background-color:#b0c0ff">2</span></b>.
		</tt></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Note: if a test doesn't finish quickly (i.e. in less
			than 30 seconds), press Ctrl-C and move onto the next
			test.
		</td>
	</tr>
	<tr>
		<td><tt># time i2cdetect <b><span style="background-color:#b0c0ff">0</span></b></tt></td>
		<td><i>Run the first I2C test</i></td>
	</tr>
	<tr>
		<td colspan="2" nowrap><tt>
&nbsp;&nbsp;WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
&nbsp;&nbsp;I will probe file /dev/i2c-0<br>
&nbsp;&nbsp;You have five seconds to reconsider and press CTRL-C!<br>
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0&nbsp;&nbsp;1&nbsp;&nbsp;2&nbsp;&nbsp;3&nbsp;&nbsp;4&nbsp;&nbsp;5&nbsp;&nbsp;6&nbsp;&nbsp;7&nbsp;&nbsp;8&nbsp;&nbsp;9&nbsp;&nbsp;a&nbsp;&nbsp;b&nbsp;&nbsp;c&nbsp;&nbsp;d&nbsp;&nbsp;e&nbsp;&nbsp;f<br>
00: XX XX XX XX XX XX XX XX XX XX <b><span style="background-color:#b0ffc0">0a</span></b> XX XX XX XX XX<br>
10: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
20: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
30: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
40: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
50: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
60: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
70: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
<br>
real&nbsp;&nbsp;&nbsp;&nbsp;0m5.692s<br>
user&nbsp;&nbsp;&nbsp;&nbsp;0m0.002s<br>
sys&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0m0.673s
		</tt></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Note the <b><span style="background-color:#b0ffc0">i2c
			bus addresses</span></b> of any active devices in the
			test above.  In this case, the only active device is at
			address <tt><b><span
			style="background-color:#b0ffc0">0x0a</span></b></tt>.
		</td>
	</tr>
	<tr>
		<td><tt># time i2cdetect <b><span style="background-color:#b0c0ff">1</span></b></tt></td>
		<td><i>Run the second I2C test</i></td>
	</tr>
	<tr>
		<td colspan="2"><tt>
&nbsp;&nbsp;WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
&nbsp;&nbsp;I will probe file /dev/i2c-1<br>
&nbsp;&nbsp;You have five seconds to reconsider and press CTRL-C!<br>
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0&nbsp;&nbsp;1&nbsp;&nbsp;2&nbsp;&nbsp;3&nbsp;&nbsp;4&nbsp;&nbsp;5&nbsp;&nbsp;6&nbsp;&nbsp;7&nbsp;&nbsp;8&nbsp;&nbsp;9&nbsp;&nbsp;a&nbsp;&nbsp;b&nbsp;&nbsp;c&nbsp;&nbsp;d&nbsp;&nbsp;e&nbsp;&nbsp;f<br>
<br>
<br>
real&nbsp;&nbsp;&nbsp;&nbsp;0m43.394s<br>
user&nbsp;&nbsp;&nbsp;&nbsp;0m0.000s<br>
sys&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0m38.392s
		</tt></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Note the <b><span style="background-color:#b0ffc0">i2c
			bus addresses</span></b> of any active devices in the
			test above.  In this case, the I2C bus was locked (the
			test failed to complete quickly), and there were no
			active devices found.
		</td>
	</tr>
	<tr>
		<td><tt># time i2cdetect <b><span style="background-color:#b0c0ff">2</span></b></tt></td>
		<td><i>Run the third I2C test</i></td>
	</tr>
	<tr>
		<td colspan="2"><tt>
&nbsp;&nbsp;WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
&nbsp;&nbsp;I will probe file /dev/i2c-2<br>
&nbsp;&nbsp;You have five seconds to reconsider and press CTRL-C!<br>
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0&nbsp;&nbsp;1&nbsp;&nbsp;2&nbsp;&nbsp;3&nbsp;&nbsp;4&nbsp;&nbsp;5&nbsp;&nbsp;6&nbsp;&nbsp;7&nbsp;&nbsp;8&nbsp;&nbsp;9&nbsp;&nbsp;a&nbsp;&nbsp;b&nbsp;&nbsp;c&nbsp;&nbsp;d&nbsp;&nbsp;e&nbsp;&nbsp;f<br>
00: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
10: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
20: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
30: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
40: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
50: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
60: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
70: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
<br>
real&nbsp;&nbsp;&nbsp;&nbsp;0m5.679s<br>
user&nbsp;&nbsp;&nbsp;&nbsp;0m0.001s<br>
sys&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0m0.678s
		</tt></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Note the <b><span style="background-color:#b0ffc0">i2c
			bus addresses</span></b> of any active devices in the
			test above.  In this case, there were no active devices
			found.
		</td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Now that the I2C buses have been scanned, dump the
			state of the active devices found using the i2cdump
			tool.
		</td>
	</tr>
	<tr>
		<td><tt># i2cdump <b><span
		style="background-color:#b0c0ff">0</span></b> <b><span
		style="background-color:#b0ffc0">0x0a</span></b></tt></td>
		<td><i>Dump the state of device </i><b><span style="background-color:#b0ffc0">0x0a</span></b><i>
		on I2C bus </i><b><span style="background-color:#b0c0ff">0</span></b><i>.</i></td>
	</tr>
	<tr>
		<td colspan="2"><tt>
No size specified (using byte-data access)<br>
&nbsp;&nbsp;WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
&nbsp;&nbsp;I will probe file /dev/i2c-0, address 0xa, mode byte<br>
&nbsp;&nbsp;You have five seconds to reconsider and press CTRL-C!<br>
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0&nbsp;&nbsp;1&nbsp;&nbsp;2&nbsp;&nbsp;3&nbsp;&nbsp;4&nbsp;&nbsp;5&nbsp;&nbsp;6&nbsp;&nbsp;7&nbsp;&nbsp;8&nbsp;&nbsp;9&nbsp;&nbsp;a&nbsp;&nbsp;b&nbsp;&nbsp;c&nbsp;&nbsp;d&nbsp;&nbsp;e&nbsp;&nbsp;f&nbsp;&nbsp;&nbsp;&nbsp;0123456789abcdef<br>
00: 00 13 00 1b 58 80 00 c0 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;.?.?X?.?........<br>
10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
20: 00 00 4a 54 60 aa 00 00 00 00 00 06 02 ff ff ff&nbsp;&nbsp;&nbsp;&nbsp;..JT`?.....??...<br>
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff&nbsp;&nbsp;&nbsp;&nbsp;................<br>
40: 2c 1f 00 00 1f 00 00 04 06 04 00 1e 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;,?..?..???.?....<br>
50: 00 ff ff ff 00 00 00 18 00 00 00 00 00 00 01 ff&nbsp;&nbsp;&nbsp;&nbsp;.......?......?.<br>
60: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff&nbsp;&nbsp;&nbsp;&nbsp;................<br>
70: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff&nbsp;&nbsp;&nbsp;&nbsp;................<br>
80: 02 22 04 99 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;?"??............<br>
90: 00 aa 55 0f f0 f0 f0 f0 f0 f0 f0 03 f1 ff ff 00&nbsp;&nbsp;&nbsp;&nbsp;.?U??????????...<br>
a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
e0: 00 00 18 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;..?.............<br>
f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00&nbsp;&nbsp;&nbsp;&nbsp;................<br>
<br>
real&nbsp;&nbsp;&nbsp;&nbsp;0m6.143s<br>
user&nbsp;&nbsp;&nbsp;&nbsp;0m0.000s<br>
sys&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0m1.138s
		</tt></td>
	</tr>
	<tr>
		<td><tt># rmmod tl880</tt></td>
		<td><i>Unload the driver from the kernel</i></td>
	</tr>
	<tr>
		<td><tt># dmesg</tt></td>
		<td><i>Display the kernel messages from the driver</i></td>
	</tr>
	<tr>
		<td colspan="2"><tt>
tl880: device found, irq 5<br>
tl880: initializing card number 0<br>
tl880: mem: 0xdc000000(0xe5d4e000), reg: 0xe2000000(0xe9d4f000), unk: 0xe2100000(0xe9e50000)<br>
tl880: Found MIT Inc. MyHD MDP110B<br>
tl880: set vip 0<br>
tl880: set vip 1<br>
i2c-dev.o: Registered 'tl880 0-1' as minor 0<br>
i2c-core.o: adapter tl880 0-1 registered as adapter
<b><span style="background-color:#b0c0ff">0</span></b>.<br>
i2c-dev.o: Registered 'tl880 0-2' as minor 1<br>
i2c-core.o: adapter tl880 0-2 registered as adapter
<b><span style="background-color:#b0c0ff">1</span></b>.<br>
i2c-dev.o: Registered 'tl880 0-3' as minor 2<br>
i2c-core.o: adapter tl880 0-3 registered as adapter
<b><span style="background-color:#b0c0ff">2</span></b>.<br>
tl880: deinitializing card number 0<br>
i2c-core.o: adapter unregistered: tl880 0-1<br>
i2c-core.o: adapter unregistered: tl880 0-2<br>
i2c-core.o: adapter unregistered: tl880 0-3<br>
tl880: module cleanup complete
		</tt></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
		Now that you've run all the tests, e-mail the results to the
		mailing list (myhd-develop at lists.sourceforge.net).  An
		example e-mail would look like this:
		</td>
	</tr>
	<tr>
		<td><tt>$ mail <i>myhd-develop at
		lists.sourceforge.net</i></tt></td>
		<td><i>Replace " at " with "@" to send the message</i></td>
	<tr>
		<td colspan="2"><tt><font size="-1">
Subject: Driver I2C test results (example)<br>
*** i2cdetect results ***<br>
time i2cdetect 0<br>
  WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
  I will probe file /dev/i2c-0<br>
  You have five seconds to reconsider and press CTRL-C!<br>
<br>
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f<br>
00: XX XX XX XX XX XX XX XX XX XX 0a XX XX XX XX XX<br>
10: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
20: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
30: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
40: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
50: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
60: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
70: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
<br>
real    0m5.692s<br>
user    0m0.002s<br>
sys     0m0.673s<br>
<br>
<br>
time i2cdetect 1<br>
  WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
  I will probe file /dev/i2c-1<br>
  You have five seconds to reconsider and press CTRL-C!<br>
<br>
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f<br>
<br>
<br>
real    0m43.394s<br>
user    0m0.000s<br>
sys     0m38.392s<br>
<br>
<br>
time i2cdetect 2<br>
  WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
  I will probe file /dev/i2c-2<br>
  You have five seconds to reconsider and press CTRL-C!<br>
<br>
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f<br>
00: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
10: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
20: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
30: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
40: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
50: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
60: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
70: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX<br>
<br>
real    0m5.679s<br>
user    0m0.001s<br>
sys     0m0.678s<br>
<br>
<br>
*** i2cdump results ***<br>
time i2cdump 0 0xa<br>
No size specified (using byte-data access)<br>
  WARNING! This program can confuse your I2C bus, cause data loss and worse!<br>
  I will probe file /dev/i2c-0, address 0xa, mode byte<br>
  You have five seconds to reconsider and press CTRL-C!<br>
<br>
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef<br>
00: 00 13 00 1b 58 80 00 c0 00 00 00 00 00 00 00 00    .?.?X?.?........<br>
10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
20: 00 00 4a 54 60 aa 00 00 00 00 00 01 fc ff ff ff    ..JT`?.....??...<br>
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................<br>
40: 2c 1f 00 00 1f 00 00 04 06 04 00 1e 00 00 00 00    ,?..?..???.?....<br>
50: 00 ff ff ff 00 00 00 18 00 00 00 00 00 00 01 ff    .......?......?.<br>
60: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................<br>
70: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................<br>
80: 02 22 04 99 00 00 00 00 00 00 00 00 00 00 00 00    ?"??............<br>
90: 00 aa 55 0f f0 f0 f0 f0 f0 f0 f0 07 fa fa fa 00    .?U????????????.<br>
a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
e0: 00 00 18 00 00 00 00 00 00 00 00 00 00 00 00 00    ..?.............<br>
f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................<br>
<br>
real    0m6.143s<br>
user    0m0.000s<br>
sys     0m1.138s<br>
<br>
<br>
*** kernel messages ***<br>
tl880: device found, irq 5<br>
tl880: initializing card number 0<br>
tl880: mem: 0xdc000000(0xe5d4e000), reg: 0xe2000000(0xe9d4f000), unk:<br>
0xe2100000(0xe9e50000)<br>
tl880: Found MIT Inc. MyHD MDP110B<br>
tl880: set vip 0<br>
tl880: set vip 1<br>
i2c-dev.o: Registered 'tl880 0-1' as minor 0<br>
i2c-core.o: adapter tl880 0-1 registered as adapter 0.<br>
i2c-dev.o: Registered 'tl880 0-2' as minor 1<br>
i2c-core.o: adapter tl880 0-2 registered as adapter 1.<br>
i2c-dev.o: Registered 'tl880 0-3' as minor 2<br>
i2c-core.o: adapter tl880 0-3 registered as adapter 2.<br>
tl880: deinitializing card number 0<br>
i2c-core.o: adapter unregistered: tl880 0-1<br>
i2c-core.o: adapter unregistered: tl880 0-2<br>
i2c-core.o: adapter unregistered: tl880 0-3<br>
tl880: module cleanup complete<br>
<br>
.
		</font></tt></td>
	</tr>
</table>

<hr size="1" width="95%" align="center">
<a name="colorbartest"></a><h4>Color Bars Sync Test</h4>
<p>The TL880 chip has an integrated simple vertical color bars generator that
can be activated by the TL880 Linux driver and tools.  I recommend using an
external monitor for this test, but using a single monitor with the video
passthrough feature of the HDTV card will be sufficient.

<table width="80%" align="center" border="0" cellspacing="15" cellpadding="0">
	<tr>
		<td><tt>$ lynx http://myhd.sourceforge.net/download.php</tt></td>
		<td><i>Download the current driver</i></td>
	</tr>
	<tr>
		<td><tt>$ tar -zxvf myhd-<i>version</i>.tar.gz</tt></td>
		<td><i>Extract the driver tarball</i></td>
	</tr>
	<tr>
		<td><tt>$ cd myhd-<i>version</i></tt></td>
		<td><i>Change to the driver source directory</i></td>
	</tr>
	<tr>
		<td><tt>$ make</tt></td>
		<td><i>Compile the kernel module (requires kernel source) and
		tools</i></td>
	</tr>
	<!--
	<tr>
		<td><tt># modprobe i2c-dev; modprobe i2c-algo-bit</tt></td>
		<td><i>Load the kernel modules required by the driver</i></td>
	</tr>
	-->
	<tr>
		<td><tt># dmesg -c</tt></td>
		<td><i>Clear the kernel message log</i></td>
	</tr>
	<tr>
		<td><tt># insmod tl880.ko</tt></td>
		<td><i>Load the kernel module into the running kernel</i></td>
	</tr>
	<tr>
		<td><tt># driver/tl880_mkdev</tt></td>
		<td><i>Create /dev/tl880/* entries for each card in the
		system</i></td>
	</tr>
	<!--
	<tr>
		<td><tt>$ cd ../tools; make</tt></td>
		<td><i>Compile the basic tools like register read/write</i></td>
	</tr>
	<tr>
		<td><tt>$ cd dpc; make</tt></td>
		<td><i>Compile the sync setting tools</i></td>
	</tr>
	-->
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
		If the display goes blank or distorted, disconnect your monitor
		from the video output of the HDTV card and connect it directly
		to your video card.  In this highly unlikely case, shutting
		down and restarting your computer will reset the hardware, and
		you can then reconnect your monitor to the HDTV card's video
		output, and the video input of the card to your video card.
		</td>
	</tr>
	<tr>
		<td><tt>$ ./dpcaux 0 0 0; sleep 10; ./videoout 0</tt></td>
		<td><i>Set color bars for 10 seconds</i></tt>
	</tr>
	<tr>
		<td><tt># rmmod tl880</tt></td>
		<td><i>Unload the driver from the kernel</i></td>
	</tr>
	<tr>
		<td colspan="2" style="background-color:#eeeeee">
			Now send an e-mail to the mailing list (myhd-develop at
			lists.sourceforge.net) saying whether you saw vertical
			color bars at 1024x768 60.0Hz.  Here are two example
			messages, one indicating success, the other failure.
		</td>
	</tr>
	<tr>
		<td><tt>$ mail <i>myhd-develop at
		lists.sourceforge.net</i></tt></td>
		<td><i>Replace " at " with "@" to send the message</i></td>
	<tr>
		<td colspan="2"><tt><font size="-1">
Subject: Driver color bars test results (example)<br>
The display showed color bars at 60.0Hz vertical (48.3kHz horizontal)<br>
refresh rate for ten seconds, then original video output was restored.<br>
<br>
.
		</font></tt></td>
	</tr>
	<tr>
		<td colspan="2" align="center" style="background-color:#eeeeee">
			- or -
		</td>
	</tr>
	<tr>
		<td colspan="2"><tt><font size="-1">
Subject: Driver color bars test results (example)<br>
The display went blank and did not switch back to original video output
after<br>
any amount of time.  I had to connect my monitor directly to my video card
to<br>
get video output back.<br>
<br>
.
		</font></tt></td>
	</tr>
</table>


<?
	display_footer();
	// vim:tw=80
?>
