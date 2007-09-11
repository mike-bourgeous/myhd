<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("", "");
?>
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

<?
	display_footer();
	// vim:tw=80
?>
