<?php

//
// Error Codes:
//
// 1 -> Accept Request File could not be deleted
//
// 9 -> Success.
//

$filename = "../../users/".$_POST['uname1']."/accepts/".$_POST['uname2'].".txt";
unlink($filename) or die("1");
die("9");
?>