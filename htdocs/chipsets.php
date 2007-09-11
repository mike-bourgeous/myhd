<?php
ini_set("include_path", "/home/groups/m/my/myhd/htdocs/:/home/groups/m/my/myhd/htdocs/include:./");
require("functions.inc");
display_header("", "");
?>

<center><h1>Chipsets of TL880-based Cards</h1></center>
<center><font size="-2"><i>Updated Dec. 2004 MB</i></font></center>

<h2>MyHD Chip List</h2>
<font size="-1">Provided by Mike Bourgeous</font>
<ul>
	<li>Zoran/Oak/Teralogic <a href="http://www.zoran.com/products/dtv/ic/tl880.html">TL880</a> HDTV decoder
	<li>Philips <a href="http://www.components.philips.com/InformationCenter/Global/FArticleDetail.asp?lArticleId=1780&lNodeId">FCV1236D/F</a> (or just 1236D) Tuner
	<li>ATI/NxtWave <a href="http://www.ati.com/products/nxt2000/">NXT2000</a> Demodulator
	<li>Micronas <a href="http://www.micronas.com/products/documentation/consumer/vpx322xe/index.php">VPX3226E</a> Video Decoder
	<li>Micronas <a href="http://www.micronas.com/products/documentation/consumer/msp34x0g/index.php">MSP3440G</a> Audio Processor
	<li>MicroClock <a href="http://www.icst.com/summary/mk2772-01.htm">MK2772-01S</a> VCXO and Set-top Clock Synthesizer
	<li>MicroClock <a href="http://www.icst.com/products/summary/mk2716.htm">MK2716S</a> HDTV Clock Synthesizer
	<li>Samsung <a href="http://www.samsungusa.com/cgi-bin/nabc/semiconductors/search/productview.jsp?prd_code=33128">K4S643232E-TC70</a> 8MB x2
	<li>Atmel <a href="http://www.atmel.com/dyn/products/product_card.asp?part_id=1913">93C46</a> Serial EEPROM
	<li>IDT <a href="http://www.idt.com/docs/QS3384_DS_27540.pdf">QS3384Q</a> 10-bit Bus Switch
	<li>National Semiconductor <a href="http://www.national.com/pf/DS/DS8921.html">DS8921M</a> Differential Line Driver and Receiver Pair
	<li>National Semiconductor <a href="http://www.national.com/pf/LM/LMV358.html">LMV358M</a> Low Voltage Op Amp
	<li>Texas Instruments <a href="http://www-s.ti.com/sc/psheets/scbs703h/scbs703h.pdf">LVTH125</a> Low Voltage Quad Buffer
	<li>NEC C3206G
</ul>

<h2>HiPix Chip List</h2>
<font size="-1">Taken from <a href="http://www.schmelzer.org/dschmelzer/hipix_chips.htm">http://www.schmelzer.org/dschmelzer/hipix_chips.htm</a></font>
<ul>
	<li>TL880; Not much in the way of documentation (HDTV decoder engine)
	<li>Temic (Microtune) 4042 Tuner; request documentation from web site
	<li>TDA6190X; docs only available for TDA6190S (IF downconvertor)
	<li>OR51210HQC (No documentation) (demodulator)
	<li>VPX 3226E (datasheet) (video decoder)
	<li>CS5330AKS (Stereo ADC)
	<li>CS4341 (Stereo DAC)
	<li>ADV7176AKS (Video encoder)
	<li>MK2772-01S (clock generator)
	<li>MK2716S (HDTV clock generator)
	<li>TI75179B (Differential driver and receiver pair)
	<li>HY57V161610D 2MB x8
	<li>EPCOS X6965M (SAW--44 MHz bandpass filter)
	<li>EPCOS 6931Q (SAW)
	<li>MEI MXFM_1006
	<li>LVC16244 (Bus buffer)
</ul>

<h2>AccessDTV Chip List</h2>
<font size="-1">Taken from <a href="http://www.schmelzer.org/dschmelzer/accessdtv_chips.htm">http://www.schmelzer.org/dschmelzer/accessdtv_chips.htm</a></font>
<ul>
	<li>TL880; Not much in the way of documentation (HDTV decoder engine)
	<li>Philips FCV1236D/F; docs available upon request (tuner)
	<li>NXT2000; Request datasheet from NxtWave (demodulator)
	<li>VPX 3225D (datasheet) (more docs) (video decoder)
	<li>i21152AB (datasheet) (PCI-PCI bridge)
	<li>SAA7146A (PCI bridge; scaler)
	<li>TDA9851T (Stereo decoder)
	<li>EPM3032A (EEPROM PLD)
	<li>MK3771-17R (HDTV clock generator)
	<li>QS3384Q (Bus switch) 
	<li>FP04AB
	<li>HY57V653220B 8MB x2
</ul>

<h2>WinTV-HD Chip List</h2>
<font size="-1">Adapted from information posted to mailing list by Mac Michaels and found on various web pages</font>
<ul>
	<li>Zoran/Oak/Teralogic <a href="http://www.zoran.com/products/dtv/ic/tl880.html">TL880</a> HDTV decoder
	<li>Philips <a href="http://www.components.philips.com/InformationCenter/Global/FArticleDetail.asp?lArticleId=1780&lNodeId">FCV1236D/F</a> (or just 1236D) Tuner <i>(Not verified)</i>
	<li>ATI/NxtWave <a href="http://www.ati.com/products/nxt2000/">NXT2000</a> Demodulator
	<li>Micronas <a href="http://www.micronas.com/products/documentation/consumer/vpx322xe/index.php">VPX3226E</a> Video Decoder
	<li>Micronas <a href="http://www.micronas.com/products/documentation/consumer/">MSP3438G</a> Audio Processor <i>Please post to the mailing list if you are aware of a more specific URL</i>
	<li>IDT <a href="http://www.idt.com/docs/QS3384_DS_27540.pdf">QS3384Q</a> 10-bit Bus Switch
	<li>Alliance Semiconductor AS7C31024-15JC 128K x 8 CM0S SRAM, 15ns access time.

<?
	display_footer();
?>
