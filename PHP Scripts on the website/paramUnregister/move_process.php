<?php

//
// Error Codes:
//
// 0 -> File error
//

$fileLocation1 = "../../users/".$_POST['uname1']."/playTemp/param.txt";
$file = fopen($fileLocation1, "r") or die("0");
fclose($file);
$param = @file_get_contents($fileLocation1);
unlink($fileLocation1);

die($param);

?>