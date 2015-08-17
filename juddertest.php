<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver - Judder Test for Linux</title>
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
			<a href="/index.html">Home</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/status.html">Development Status</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/howtohelp.html">How to Help</a>
		</td></tr>
<!--
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/testcd.html">Bootable Test CD</a>
		</td></tr>
-->
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/tl880spy.html">TL880Spy Tool</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/myirc.html">MyHD IR Receiver</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/juddertest.html">Judder Test for Linux</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/download.html">Downloads</a>
		</td></tr>
		<tr><td align="center" valign="middle" class="menuitem">
			<a href="/chipsets.html">Card Chipset List</a>
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
			<i>New TL880Spy release:</i><br><a href="/download.html#spy1.3">TL880Spy 1.3</a><br>
			<font size="-2">Aug. 11, 2007 16:25:00 MDT</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New release:</i><br><a href="/download.html#0.2.3">MyHD 0.2.3</a><br>
			<font size="-2">Mar. 28, 2007 02:20:00 MDT</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New release:</i><br><a href="/download.html#0.2.2">MyHD 0.2.2</a><br>
			<font size="-2">Jan. 23, 2007 00:35:00 MST</font>
		</td></tr>
		<tr><td align="center" valign="middle" class="newsitem">
			<i>New tool release:</i><br><a href="/download.html#spy1.2">TL880Spy 1.2</a><br>
			<font size="-2">Jan. 19, 2007 19:30:00 MST</font>
		</td></tr>
	</table>
</center>

					<br><br><br><br>Hosted by:<br>
					<A href="http://sourceforge.net"><IMG src="http://sourceforge.net/sflogo.php?group_id=75027&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"></A>
				</td>
				<td width="100%" valign="top" class="mainclass">
				<center><img src="/images/myhd.png" alt="Judder Test for Linux"></center><br>

<!-- Begin main text -->

<center><h3>Judder Test for Linux</h3></center>

<center><b><a href="/download.html#judder">Click here</a></b> to download the latest version.</center>
<center><b><a href="/juddertest_shots.html">Click here</a></b> to view screen shots.</center>

<br><center><i>For discussion on <b>Judder Test for Linux</b>, see the
<a href="http://www.avsforum.com/avs-vb/showthread.php?t=625481">AVSForum thread</a> or the 
<a href="http://www.sourceforge.net/mail/?group_id=75027">myhd-develop</a> mailing list.</i></center>
<p>This program is designed to allow a home theater installer or hobbyist to test
for tearing or judder artifacts on a display at a specific refresh rate.  It
is limited only by the fill rate of the system's video card.  To use a specific
refresh rate, make sure that the only rate available to X in /etc/X11/xorg.conf
or /etc/X11/XF86Config-4 is the rate desired.  Press "ESC" to exit the program.

<p>After the test is finished, some information about the test will be printed to
the terminal, such as frames drawn, vertical retraces elapsed, frames per
second, frames dropped (below refresh) or skipped (above refresh), etc.  If the
"frames dropped" number is greater than 1 when running without any options, the
system is too slow for the judder test.  As Judder Test for Linux uses OpenGL,
a faster video card will likely solve the problem.

<p>If your system is fast enough (and chances are it is) the judder test will sync
perfectly to the refresh rate, allowing you to test interlaced modes as well.
The various output modes would allow testing an external video processor, for
example.

<p><b>Run <code>make</code> to compile, <code>./judder</code> to run, <code>make
install</code> to copy to /usr/bin/.</b>

<p>I highly recommend checking out cool mode!

<p><pre><b>usage:</b>
	./judder [direction [step [skip [bars [perspective [goofy]]]]]]<br>

<b>where:</b>
	direction is 0 for horizontal, 1 for vertical, or 2 for diagonal
	step is the number of pixels to move bars, up to +/- the size of each bar
	skip is the number of retraces to skip each frame, 0 for default, -1 for no sync
	bars is the number of bars to display, 0 for default
	perspective is 0 for normal, 1 for 3D
	goofy is 0 for normal, 1 for goofy

<b>environment variables:</b>
	XRES, YRES: custom resolution
	FS: 1 for full screen, 0 for window (default is 1)

<b>examples:</b>
	default parameters:
		./judder
	vertical scroll:
		./judder 1
	display refresh testing (adjust for xres) (<b>warning - potential headache</b>):
		./judder 0 128 0 4
	fill rate testing (will have tearing):
		./judder 0 1 -1 64
	cool mode:
		./judder 1 0 0 0 1
	goofy mode:
		./judder 2 2 0 4 1 1
	custom resolution:
		XRES=1280 YRES=720 ./judder
	windowed mode:
		FS=0 ./judder
</code>

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

