<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver - Downloads</title>
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
				<center><img src="/images/myhd.png" alt="Downloads"></center><br>

<!-- Begin main text -->


<br>
<a href="http://www.sourceforge.net/project/showfiles.php?group_id=75027">SourceForge File Releases Page</a> - All file releases<br>
<a href="http://myhd.sourceforge.net/myhd-debuglogs.tar.bz2">Windows Driver Debug Logs</a> - Register dumps and debug logs from the MyHD Windows driver<br>
<a href="/download.php#driver">MyHD/TL880 Driver</a> - TL880 driver releases<br>
<a href="/download.php#tl880spy">TL880Spy</a> - TL880 card tweak tool for Windows<br>
<a href="/download.php#judder">Judder Test for Linux</a> - Tool for testing display/video card interaction

<!-- Driver -->
<a name="driver"></a><center><h1>Download TL880 Driver</h1></center>

<a name="0.2.3"></a><h3>MyHD 0.2.3</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;Many internal improvements in this version, 
	though few new user-worthy features.  Improvements to card
	initialization.  New ioctl interfaces.  Improved VPX chip support.
	More chip features documented.  See the ChangeLog for the full details.
	<br>All interrupt issues (IRQ sharing, etc.) should be resolved.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Use this if you want to help
	with driver development and testing.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=496980">myhd-0.2.3</a> - SourceForge release
	<li>myhd-0-2-3 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>



<a name="0.2.2"></a><h3>MyHD 0.2.2</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This release adds support for kernels through
	2.6.19, fixes interrupts, and adds a tool for testing ancillary drivers
	(such as the tuner and demodulator chips).
	Framebuffer image viewing is possible with the latest version of
	<a href="http://linux.bytesex.org/fbida/">fbi</a>, the framebuffer
	image viewer.  X and the Linux Console do not yet work with the
	framebuffer driver.  Video playback requires an as-yet unavailable
	patch to mplayer.  Video capture is not yet possible.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Use this if you want to help
	with driver development and testing.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;The TL880 card should not be sharing an interrupt line with the video
	card for best results.  It is best if the card has its own interrupt.  It may be
	necessary to move the card to a different PCI slot in your computer if
	your motherboard shares IRQ lines.  If you have no issues with shared
	IRQs, please let me know your hardware configuration and /proc/interrupts.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=480677">myhd-0.2.2</a> - SourceForge release
	<li>myhd-0-2-2 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>



<a name="0.2.1"></a><h3>MyHD 0.2.1</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This version adds a preliminary framebuffer
	driver, allowing image and video playback (albeit at a very slow
	framerate).  Image viewing is possible with the latest version of
	<a href="http://linux.bytesex.org/fbida/">fbi</a>, the framebuffer
	image viewer.  X and the Linux Console do not yet work with the
	framebuffer driver.  Video playback requires an as-yet unavailable
	patch to mplayer.  Video capture is not yet possible.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Use this if you want to help
	with driver development, or you want an overpriced color bars generator
	with an Open Source driver.  The color bars may or may not work on
	cards other than the MyHD.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;The TL880 card must not be sharing an interrupt line with the video
	card.  It is best if the card has its own interrupt.  It may be
	necessary to move the card to a different PCI slot in your computer if
	your motherboard shares IRQ lines.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;Interrupts do not work properly, at least with the MyHD.  Please
	do not run the tools/testdpcint.sh script or write to any registers that
	would enable interrupts.  The driver doesn't do anything with interrupts
	yet anyway, so this is not a big problem.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=437206">myhd-0.2.1</a> - SourceForge release
	<li>myhd-0-2-1 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>



<a name="0.2.0"></a><h3>MyHD 0.2.0</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This is the official release of 0.2.0.
	Interrupt support has been disabled.  The 0.2.x series is the
	first series of the TL880 driver for kernel 2.6.  This release
	adds a lot of new information to the documentation on the TL880
	chip.  It also adds new tools for manipulating the card's memory
	and registers.  There is not yet any video capture support.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Use this if you want to help
	with driver development, or you want an overpriced color bars generator
	with an Open Source driver.  The color bars may or may not work on
	cards other than the MyHD.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;The TL880 card must not be sharing an interrupt line with the video
	card.  It is best if the card has its own interrupt.  It may be
	necessary to move the card to a different PCI slot in your computer if
	your motherboard shares IRQ lines.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;Interrupts do not work properly, at least with the MyHD.  Please
	do not run the tools/testdpcint.sh script or write to any registers that
	would enable interrupts.  The driver doesn't do anything with interrupts
	yet anyway, so this is not a big problem.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=393976">myhd-0.2.0</a> - SourceForge release
	<li>myhd-0-2-0 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>



<a name="0.1.99.2"></a><h3>MyHD 0.1.99.2</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This is the second prerelase of version 0.2.0.
	This release adds minimal support for the on-screen display of images.
	So far only color test patterns are displayed behind the mouse cursor.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Only use this if you want to help
	with driver development, or you want an overpriced color bars generator
	with an Open Source driver.  The color bars may or may not work on
	cards other than the MyHD.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;The TL880 card must not be sharing an interrupt line with the video
	card.  It is best if the card has its own interrupt.  It may be
	necessary to move the card to a different PCI slot in your computer if
	your motherboard shares IRQ lines.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;Repeatedly loading and removing the driver will corrupt the PCI
	device information in the kernel.  This situation should not occur in
	normal usage, but if someone knows what causes this, please post to the
	mailing list.   Symptoms: driver will no longer load, failing with No
	Such Device.  <code>lspci</code> and <code>cat /proc/pci</code> will segfault.
	<br><br>Interrupts do not work properly, at least with the MyHD.  Please
	do not run the tools/testdpcint.sh script or write to any registers that
	would enable interrupts.  The driver doesn't do anything with interrupts
	yet anyway, so this is not a big problem.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=388311">myhd-0.1.99.2</a> - SourceForge release
	<li>myhd-0-1-99-2 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>



<a name="0.1.99.1"></a><h3>MyHD 0.1.99.1</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This is the first release with kernel 2.6
	support.  It is a preliminary testing release.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This driver is for developers and testers.
	It has no video capture capability.  Only use this if you want to help
	with driver development, or you want an overpriced color bars generator
	with an Open Source driver.  The color bars may or may not work on
	cards other than the MyHD.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;The TL880 card must not be sharing an interrupt line with the video
	card.  It is best if the card has its own interrupt.  It may be
	necessary to move the card to a different PCI slot in your computer if
	your motherboard shares IRQ lines.
	<br><br>&nbsp;&nbsp;&nbsp;&nbsp;Repeatedly loading and removing the driver will corrupt the PCI
	device information in the kernel.  This situation should not occur in
	normal usage, but if someone knows what causes this, please post to the
	mailing list.   Symptoms: driver will no longer load, failing with No
	Such Device.  <code>lspci</code> and <code>cat /proc/pci</code> will segfault.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=75430&release_id=371972">myhd-0.1.99.1</a> - SourceForge release
	<li>myhd-0-1-99-1 - CVS tag (note that anonymous CVS is at least 24 hours out of date)
</ul>
<a name="old"></a><br><hr width="25%" size="1" align="center"><center><b>Old Driver Releases</b></center>
<center><a href="/download.php?old=1#old">Show Old Releases</a></center>
<!-- TL880Spy -->
<a name="tl880spy"></a><center><h1>Download TL880Spy Card Analyzer for Windows</h1></center>

<a name="spy1.3"></a><h3>TL880Spy 1.3</h3>
<b>Release notes:</b><br>
<blockquote>
	Changes and additions:
	<ul>
		<li>The ability to compile the driver with Visual Studio 2005 and the appropriate ddk.
		<li>The register watch tool can now rapid-fire capture a register to try to capture
		  every change made to the register (eventually to be expanded to decode I2C)
		<li>A DPC2 register dump option in the RegDumper tool.
		<li>Corrected file names and extensions of register and memory dumps.
		<li>Additional information in the report header.
		<li>The report window is bigger (how do I make it resizable???).
		<li>Memory/register dump loading no longer creates a file if the selection
		    doesn't exist.
		<li>A new DPC information tool.
	</ul>
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;Only use multiples of four when looking
	for registers.  Using anything that's not a multiple of four will
	access multiple registers at once, which doesn't provide useful meaning.
	<br><br>
	&nbsp;&nbsp;&nbsp;&nbsp;This source release workspace can only be loaded in
	Visual Studio 2005.  The source files themselves may be copied into the older
	source release directory and loaded with Visual Studio 6 if necessary.
 </blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=90959&release_id=531188">TL880Spy-1.3</a> - SourceForge release
</ul>

<a name="spy1.2"></a><h3>TL880Spy 1.2</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This release adds a binary register dump feature.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;Only use multiples of four when looking
	for registers.  Using anything that's not a multiple of four might
	access multiple registers at once, which doesn't provide useful meaning.
	<br><br>
	&nbsp;&nbsp;&nbsp;&nbsp;The Windows 95 driver is no longer included in the
	binary package.  The Windows NT, 2000, and XP driver works fine.
	<br><br>
	&nbsp;&nbsp;&nbsp;&nbsp;This source release workspace can only be loaded in
	Visual Studio 2005.  The source files themselves may be copied into the older
	source release directory and loaded with Visual Studio 6 if necessary.
 </blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=90959&release_id=479867">TL880Spy-1.2</a> - SourceForge release
</ul>

<a name="spy1.1"></a><h3>TL880Spy 1.1</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This release adds several new features.  There
	is now support for accessing TL880 memory in addition to TL880
	registers.  A register and memory viewer have been added which view an
	entire 0x100 byte block at once.  The original registry viewer can now
	access any register.  The useless wizard has been disabled, so the card
	tools can be accessed directly.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;Only use multiples of four when looking
	for registers.  Using anything that's not a multiple of four might
	access multiple registers at once, which doesn't provide useful meaning.
	<br><br>
	&nbsp;&nbsp;&nbsp;&nbsp;The Windows 95 driver may not work (very few
	people are using a Windows 95 derivative anyway).  The Windows NT, 2000,
	and XP driver works fine.
 </blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=90959&release_id=393999">tl880spy-1.1</a> - SourceForge release
</ul>

<a name="spy1.0"></a><a name="spy1.0beta"></a><h3>TL880Spy 1.0beta</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This initial release of the <a href="/tl880spy.php">TL880Spy</a> Windows
	tweak tool should be fully functional.  It is based on the BtSpy tool
	from the <a href="http://btwincap.sourceforge.net/">btwincap</a> Open
	Source Windows BT8xx capture driver.  This contains modified code from
	DScaler's PCI access driver and BtSpy itself.  See the README file in
	the distribution for information on how to use this program.
</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;The wizard in this tool is mostly useless.  The
	real useful tools are available in the results page of the wizard.
	Currently there is not a way to read any random register; just a select
	few.  There is a tool to write any register, though.  Registers are not
	yet forced to multiples of four; only use multiples of four when looking
	for registers.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=90959&release_id=176332">tl880spy-1.0beta</a> - SourceForge release
</ul>

<!-- MyIRC -->
<a name="myirc"></a><center><h1>Download MyIRC Test Program</h1></center>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;A very simple program for accessing and testing an
	irman-like infrared receiver such as the MyHD MyIRC remote control receiver.</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="/myirc.tar.bz2">myirc</a> - Source code
</ul>

<!-- Judder Test -->
<a name="judder"></a><center><h1>Download Judder Test for Linux</h1></center>

<a name="judder-0.1.0"></a><h3>Judder Test 0.1.0</h3>
<b>Release notes:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;This first release of the Judder Test for Linux
	is fully functional.  See the <a href="/juddertest.php">Judder Test</a>
	page for usage information.  This is a command line tool.</blockquote>
<br>
<b>Known issues:</b><br>
<blockquote>
	&nbsp;&nbsp;&nbsp;&nbsp;If you do not have VeraBd.ttf in /usr/share/fonts/TTF,
	the system time will not be displayed at the top of the screen.  This is not a
	major issue.
</blockquote>
<br>
<b>Download it:</b><br>
<ul>
	<li><a href="http://sourceforge.net/project/showfiles.php?group_id=75027&package_id=174723&release_id=382460">juddertest-0.1.0</a> - SourceForge release
</ul>



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