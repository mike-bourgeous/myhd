<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("", "Judder Test for Linux");
?>
<center><h3>Judder Test for Linux</h3></center>

<center><b><a href="/download.php#judder">Click here</a></b> to download the latest version.</center>
<center><b><a href="/juddertest_shots.php">Click here</a></b> to view screen shots.</center>

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

<?
	display_footer();
	// vim:tw=80
?>

