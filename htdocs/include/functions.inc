<? /* -*- Mode: php -*- */ 

function display_header($image, $section)
{
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- 
	This page was created in PHP by Mike Bourgeous using the amazing Vim editor.
-->
<html>
	<head>
		<title>MyHD Linux Driver<? if($section) echo " - $section"; ?></title>
		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
		<link rel="stylesheet" type="text/css" href="/style/myhd_style.css">
	</head>
	<body bgcolor="#000000" text="black" link="#3070c0" vlink="#1060c0" class="mainclass">
		<h2 style="text-align: center; margin: 2em 0">This project has moved to <a href="https://github.com/mike-bourgeous/myhd">https://github.com/mike-bourgeous/myhd</a></h2>
		<hr style="margin-bottom: 2em">
		<table width="100%" border="0" cellspacing="2" cellpadding="1" class="pagetable" bgcolor="white">
			<tr>
				<td width="192" valign="top" align="center" class="mainclass">
				<br>
					<? require("menu.inc"); ?>
					<br><br><br><br>Hosted by:<br>
					<A href="http://sourceforge.net"><IMG src="http://sourceforge.net/sflogo.php?group_id=75027&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"></A>
				</td>
				<td width="100%" valign="top" class="mainclass">
				<center><img src="<?
	if($image != "") {
		echo "/images/$image";
	} else {
		echo "/images/myhd.png";
	}
				?>" alt="<?
	if($section != "") {
		echo "$section";
	} else {
		echo "MyHD Linux Driver";
	}
				?>"></center><br>

<!-- Begin main text -->

<?
}

function display_footer()
{
?>
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
<?
}
?>
