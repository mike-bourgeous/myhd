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
