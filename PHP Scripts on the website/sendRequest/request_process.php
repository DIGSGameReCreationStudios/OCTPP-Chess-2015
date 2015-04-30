<?php

//
// Error Codes:
//
// 0 -> Could not open file
//
// 9 -> Success.
//


$filename = "../../users/".$_POST['uname2']."/requests/request.txt";
$myfile = fopen($filename, 'w') or die("0");
fwrite($myfile, $_POST['uname1']."\n".$_POST['color']."\n".stripslashes($_POST['txt']));
fclose($myfile);

$filename = "../../users/".$_POST['uname1']."/accepts/".$_POST['uname2'].".txt";
$ff = fopen($filename, 'w') or die("0");
fwrite($ff, "UNMODIFIED");

die("9");
?>