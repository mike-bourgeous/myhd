<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver - MyHD/MyIRC Remote Receiver</title>
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
				<center><img src="/images/myhd.png" alt="MyHD/MyIRC Remote Receiver"></center><br>

<!-- Begin main text -->

<center><h3>MyHD/MyIRC Infrared Remote Control Receiver</h3></center>

<p>The MyHD is shipped with a simple, easy to use remote control and a serial or
USB infrared receiver.  Since I couldn't find a single mention of anyone using
the MyIRC receiver in Linux, I decided to do a little digging and see if I could
make it work. <i>Note: if you simply want to know how to use this receiver with
lirc, <a href="#howto">skip</a> the next few paragraphs.</i>

<p>The first thing I did was open the hood to look at the circuit.  It is a
simple circuit, which may look familiar to those with lirc experience.  The
board has one DIP8 12C509A One-Time Programmable PIC, one IR receiver, four
diodes, and two resistors.  It is basically the same as the home brew irman
receiver, right down to the PIC, as you can see from the pictures at the bottom
of this page.

<p>Unsure of how to proceed, I found a serial port traffic sniffer, PortMon from
Sysinternals, and watched the MyIRC.exe program interact with the receiver.  The
receiver is initialized by opening the serial port at 9600 baud and sending
'IR'.  The receiver then responds with 'OK'.  Remote codes are sent as 6 byte
packets.  This is identical to the irman protocol, with one potential exception:
the bits in each byte of data from the remote are reversed.  This is of little
consequence.

<p>To make sure Linux could interact with the receiver, I wrote a simple test
program to initialize the receiver and dump all remote data.  The program may be
useful to those who wish to test any irman or MyHD/MyIRC serial port receiver.
It may be downloaded <a href="/download.php#myirc">here</a>.

<p>After verifying that the receiver worked with Linux's serial driver and that
the data received by my test program was identical to that shown in my
investigation of the Windows MyIRC.exe program, I recompiled lirc with the irman
driver.

<p><a name="howto"></a><b>HOWTO:</b> Here's what it takes to get the MyHD MyIRC receiver
working in Linux with lirc:

<ul>
<li>Connect the MyHD receiver to an available serial port.
<li>Download and install lirc, making sure that the <code>irman</code> driver is
enabled.
<li>Make sure lircd uses the irman driver.  This is done by adding <code>-H
irman -d /dev/ttyS<i>?</i></code> to the lircd command line.  On Gentoo, for
example, I modified the <code>LIRCD_OPTS</code> line in /etc/conf.d/lircd to
read <code>LIRCD_OPTS="-d /dev/ttyS0 -H irman"</code>.
<li>Create a configuration file for your remote, or download
<a href="/MyHD.lirc">this one</a> for the MyHD remote.  <br><i><b>Important:</b> the
MyHD receiver is not compatible with remote definitions generated with another
receiver.  You must create your own remote definitions using irrecord, or
download definitions which were created with the MyIRC receiver.</i>
<ul><i>Creating a remote definition:</i>
<li>Make sure lircd is running
<li>Run <code>irrecord -H irman -d /dev/ttyS<i>?</i> <i>Remote_Name</i></code> in a terminal.
<li>Follow the instructions given by irrecord.
<li>Dump the data generated by irrecord into /etc/lircd.conf by running
<code>cat <i>Remote_Name</i> &gt;&gt; /etc/lircd.conf</code> as root in a
terminal.
</ul>
<li>Restart lircd.
<li>Test the configuration by running irw and pressing a few buttons on the
remote.
</ul>

<center><p><b><i>Related Links:</i></b> <a href="http://www.lirc.org/">LIRC
Home</a> - <a href="http://fly.cc.fer.hr/~mozgic/UIR/">Homebrew Universal
Infrared Receiver (UIR/irman)</a></center>




<table cellspacing="10" border="0" align="center">
<tr><td colspan="2" bgcolor="#b0c0ff">
<center><b>Inside the MyHD IR Receiver</b></center>
</td></tr>
<tr><td>
<center><font size="-1"><i>
<a href="/images/myirc_top.jpg"><img src="/images/myirc_top.jpg" border=1 width="230" height="400" alt="MyIRC Top View"></a>
<br><a href="/images/myirc_top.jpg">Top View</a>
</i></font></center>
</td>
<td>
<center><font size="-1"><i>
<a href="/images/myirc_bottom.jpg"><img src="/images/myirc_bottom.jpg"
border=1 width="240" height="383" alt="MyIRC Bottom View"></a>
<br><a href="/images/myirc_bottom.jpg">Bottom View</a>
</i></font></center>
</td>
</tr>
</table>

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

