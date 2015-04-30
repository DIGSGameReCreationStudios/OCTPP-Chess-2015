<?php

//
// Error Codes:
//
// 0 -> File could not be opened
//
// 9 -> Success.
//

$filename = "../../users/".$_POST['uname2']."/accepts/".$_POST['uname1'].".txt";
$myfile = fopen($filename, 'w') or die("0");
fwrite($myfile, $_POST['param']);

unlink("../../users/".$_POST['uname1']."/requests/report.txt");
unlink("../../users/".$_POST['uname1']."/requests/request.txt");
fclose($myfile);

die("9");

?>