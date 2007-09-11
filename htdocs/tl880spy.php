<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("tl880spy.png", "TL880Spy");
?>
<center><h3>TL880Spy Card Tweak Tool for Windows</h3></center>

<center><b><a href="/download.php#tl880spy">Click here</a></b> to download the latest version.</center>
<center><b><a href="/images/tl880spy_preview.png">Click here</a></b> to view a screen shot.</center>
<center><b><a href="/tl880spy1.0.php">Click here</a></b> to view the old
TL880Spy 1.0 page.</center>

<p>How to get started: <a href="/download.php#tl880spy">download</a> the binary
distribution, unzip it to a folder, and run TL880Spy.exe.
A result log page will be displayed, where there are several buttons: 
<ul>
<li><b>MemEditor</b> - Allows changing an arbitrary dword in card memory.
<li><b>MemViewer</b> - Displays an arbitrary 0x100 block of card memory,
allowing the user to save the displayed block to the report.
<li><b>MemDumper</b> - Saves an arbitrary block of card memory to a binary file.
<br><br>
<li><b>RegEditor</b> - Allows changing an arbitrary card register.
<li><b>RegWatcher</b> - Shows the contents of 12 arbitrary registers with a high
update frequency.
<li><b>RegDumper</b> - Saves one or more predefined blocks of registers to the
report.
<li><b>RegViewer</b> - Displays an arbitrary 0x100 block of card registers,
allowing the user to save the displayed block to the report.
<li><b>BinRegDump</b> - Saves an arbitrary block of card registers to a binary
file.
<li><b>DPC Viewer</b> - Shows the current video mode parameters and display
state.
<br><br>
<li><b>Save...</b> - Saves the report to a text file.
<li><b>Exit</b> - Exits the TL880Spy program.
</ul>

<p>RegDumper can be used to figure out all kinds of things.  For example, if you
want to figure out how to set various video modes, do a dump of the DPC 
registers and the miscellaneous registers for each video mode, and look for 
differences.  It's easiest if you delete all the text in the result log, and 
save each dump to a separate file.  Then you can just use diff -u if you have 
Cygwin installed (or can access the files in a UNIX/Linux distribution with 
GNU tools available) to find differences between the files.  

<p>Some of the registers are status registers that are updated constantly, like 
register 0x10004.  Making multiple dumps of the same region without making 
any changes in the card's software will help you to find these.

<p>For a list of known registers/register ranges, see tl880doc.txt in the driver
source distribution, or in Webcvs <a
href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/*checkout*/myhd/myhd/doc/tl880doc.txt?rev=HEAD&content-type=text/plain">here</a>.
There is also a convenient list of registers <a href="/reglist.html">here</a>,
but it is very out of date due to the difficulty of adding new information.  If
someone wants to volunteer to write a database to store and automatically
generate both files (reglist.html and tl880doc.txt), send a post to the list.

<p>As you learn about the registers, share your findings on the mailing list.  
Particular things that would be good: figuring out the formula for the PLL 
reference set in register 0x5800.  This PLL either sets or affects the pixel 
clock of the sync generator.  The first three or so hex digits seem to be a 
mutliplier (increase speed), and some of the digits further down seem to be a 
divider (decrease speed).  Make sure you don't do this test on a monitor 
that's extremely sensitive to refresh rate changes.  I did it on my Plus 
UP-1100P projector, ViewSonic G90f monitor, and old Compaq 15" monitor 
without problems.  I must say..  1024x768p at 27Hz is quite a sight to see!  
It would be great if someone who has access to a frequency counter and/or
digital oscilloscope would hook the H-Sync and V-Sync outputs to the test
hardware and determine the relative effect changing register 0x5800 has on the
refresh rate.
<p>Help figuring out the audio registers (0x3000-0x3040) would be appreciated,
as that is one current area of focus.
<p>Anyway, I hope this gets you all excited about figuring out the card.  Don't
change anything in the HIF region (0x0 to 0x14), as that could cause the card 
to start interrupting the CPU without the driver expecting it, resulting in a 
system hang.  Also, anything from 0x18000 to 0x20000 or so may be 
dangerous to change (firmware resides in that area).

<?
	display_footer();
	// vim:tw=80
?>
