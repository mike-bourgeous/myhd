<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("tl880spy.png", "TL880Spy");
?>
<center><h3>TL880Spy Card Tweak Tool for Windows</h3></center>

<center><b><a href="/download.php#tl880spy">Click here</a></b> to download the latest version.</center>
<center><b><a href="/images/tl880spy_preview.png">Click here</a></b> to view a screen shot.</center>

<!--
<p><b>Until I get some proper documentation written for this tool, here's the
announcement made to the mailing list.  It contains some information to help you
get started.  Most people who are tech-savvy enough to install an HDTV card
will be able to figure this software out; it's not hard.</b>
-->

<p>How to get started: <a href="/download.php#spy1.0">download</a> the binary distribution 
(tl880spy1.0.zip), unzip it to a folder, and run TL880Spy.exe.  You can 
follow the directions in the wizard if you want, or you can just skip it.  
The real fun happens on the results page, where there are three buttons: 
RegEditor, RegViewer, and RegDumper.  RegEditor allows you to set any 
register to any value (use multiple of four register offsets, as the registers
are four bytes long each).  RegViewer shows a realtime view of a few preselected
registers (selectable registers for realtime view will be in the next version).
RegDumper presents a number of check boxes to select ranges of registers to dump
to the TL880Spy result log.

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

<p>As you learn about the registers, share your findings on the mailing list.  
Particular things that would be good: figuring out the formula for the PLL 
reference set in register 0x5800.  This PLL either sets or affects the pixel 
clock of the sync generator.  The first three or so hex digits seem to be a 
mutliplier (increase speed), and some of the digits further down seem to be a 
divider (decrease speed).  Make sure you don't do this test on a monitor 
that's extremely sensitive to refresh rate changes.  I did it on my Plus 
UP-1100P projector, ViewSonic G90f monitor, and old Compaq 15" monitor 
without problems.  I must say..  1024x768p at 27Hz is quite a sight to see!  
Another great set of registers to figure out would be the OSD context, 
located between 0x10080 and 0x100ac.  So far it seems that 0x10080 has 
something to do with the color palette, and 0x10084 selects OSD images.  
0x10084 seems to ignore the lower 4 bits (lowest hex digit) of the register, 
so only changes in multiples of 16 decimal (0x10 hexadecimal) will have an 
effect.

<p>Anyway, I hope this gets you all excited about figuring out the card.  Don't
change anything in the HIF region (0x0 to 0x14), as that could cause the card 
to start interrupting the CPU without the driver expecting it, resulting in a 
system hang.  Also, anything from 0x18000 to 0x20000 or so may be 
dangerous to change (firmware resides in that area).

<?
	display_footer();
	// vim:tw=80
?>
