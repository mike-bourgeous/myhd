<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver - TL880Spy</title>
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
		<link rel="stylesheet" type="text/css" href="/style/myhd_style.css">
	</head>
	<body bgcolor="#000000" text="black" link="#3070c0" vlink="#1060c0" class="mainclass">
		<table width="100%" border="0" cellspacing="2" cellpadding="1" class="pagetable" bgcolor="white">
			<tr>
				<td width="192" valign="top" align="center" class="mainclass">
				<br>
					
<!-- Menu -->
<center>
	<table width="200" border="0" cellspacing="1" cellpadding="0" class="mainclass" bgcolor="white">
		<tr><td align="center" valign="middle" class="menuheader">
			<b>MyHD Linux Driver</b>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/index.php">Home</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/status.php">Development Status</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/howtohelp.php">How to Help</a>
		</td></tr>
<!--
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/testcd.php">Bootable Test CD</a>
		</td></tr>
-->
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/tl880spy.php">TL880Spy Tool</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/myirc.php">MyHD IR Receiver</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/juddertest.php">Judder Test for Linux</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/download.php">Downloads</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/chipsets.php">Card Chipset List</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="http://www.sourceforge.net/mail/?group_id=75027">Mailing Lists</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="http://www.sourceforge.net/projects/myhd/">SourceForge Project</a>
		</td></tr>
	</table>
</center>

<br>

<!-- News -->
<center>
	<table width="192" border="0" cellspacing="1" cellpadding="0" class="mainclass" bgcolor="white">
		<tr><td align="center" valign="middle" class="menuheader">
			<b>News</b>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New TL880Spy release:</i><br><a href="/download.php#spy1.3">TL880Spy 1.3</a><br>
			<font size="-2">Aug. 11, 2007 16:25:00 MDT</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New release:</i><br><a href="/download.php#0.2.3">MyHD 0.2.3</a><br>
			<font size="-2">Mar. 28, 2007 02:20:00 MDT</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New release:</i><br><a href="/download.php#0.2.2">MyHD 0.2.2</a><br>
			<font size="-2">Jan. 23, 2007 00:35:00 MST</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New tool release:</i><br><a href="/download.php#spy1.2">TL880Spy 1.2</a><br>
			<font size="-2">Jan. 19, 2007 19:30:00 MST</font>
		</td></tr>
	</table>
</center>

					<br><br><br><br>Hosted by:<br>
					<A href="http://sourceforge.net"><IMG src="http://sourceforge.net/sflogo.php?group_id=75027&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"></A>
				</td>
				<td width="100%" valign="top" class="mainclass">
				<center><img src="/images/tl880spy.png" alt="TL880Spy"></center><br>

<!-- Begin main text -->

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

<!-- End main text -->
				</td>
			</tr>
		</table>
		<table width="100%" border="0" class="credit" bgcolor="white">
			<tr>
				<td class="mainclass">
					<center>&copy;2006 MyHD Linux Driver Project</center>
					<!-- Common law copyrights are claimed -->
				</td>
			</tr>
		</table>
	</body>
</html>