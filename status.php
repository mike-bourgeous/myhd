<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver</title>
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
				<center><img src="/images/myhd.png" alt="MyHD Linux Driver"></center><br>

<!-- Begin main text -->

<center><h3>Development Status</h3></center>
<p>The TL880 driver does not support video capture or normal playback at this 
time, but we need as many people as possible to run tests to help the driver move
along.

<p>The tools support turning on the display of an external monitor at a few
resolutions and refresh rates.  The included framebuffer driver is hard coded
at 1024x768, 60Hz, 32-bit color.

<p>The Linux driver and tools are capable of loading and displaying images
on the TL880's OSD.  View the following video to see exactly what can be done.
Much work is still needed, but progress is being made quickly.  Just a few
months ago it wasn't possible to display anything but the built-in color bars
and a mouse pointer.  The included framebuffer driver allows the latest version
of the <a href="http://linux.bytesex.org/fbida/">fbi</a> image viewer to play
slideshows and display most image formats.  The video below was made before
the framebuffer driver was written.

<blockquote>
<a href="video/osdtest.avi">Old OSD Test Video</a> (FFMPEG4) - generated with:
<pre>
tools $ /osd.sh ; dpc/videoout 1 ; ./iocwritereg 10000 3f ; ./osdimage cloud.raw ; 
	sleep 2 ; ./osdimage ~/src/suzi/suzi.Y ; sleep 1 ; ./writemem 2d8000 5ff0000b ; 
	sleep 2 ; ./writemem 2d8008 218000
</pre>
</blockquote>

<p>It is possible to read from, write to, and otherwise alter the TL880's
registers and memory using command-line tools in Linux and the TL880Spy
graphical tool in Windows.

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
