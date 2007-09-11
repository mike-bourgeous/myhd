<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("", "");
?>
<center><h3>Linux driver for TL880-based HDTV tuner cards</h3></center>
<p>This is the development page for the Linux driver for the MyHD and other 
HDTV tuners based on the TL880 chip.  The driver is in early development stage
right now, and we need lots of help testing.  If you own any TL880-based card,
please download the driver using the <a href="/download.php#driver">Downloads</a> link at the left, follow
the instructions given on the <a href="/howtohelp.php">How to Help</a> page to run a test on your
card, and post the results to the mailing list.  Keep in mind that the driver 
does not support video capture or playback at this time, but we need as many
people as possible to run tests to help the driver move along.  Please see the
mailing lists and the links at the left for more information.

<p>This page also includes information, tools, and resources related to home
theater PC's and high-end multimedia computing.

<!--
<p>Note: as of Oct. 11th, 2005, Mike Bourgeous, the maintainer of the MyHD Linux
driver, is back, and development will resume shortly.
<p>Note: from Oct. 8th, 2003 to some time around Nov. 2005, Mike Bourgeous is
not maintaining this site.  Updates during that time, if any, will come from
another maintainer.  Please read the mailing list archives for details.
-->

<?
	display_footer();
	// vim:tw=80
?>
