<?php

//
// Error Codes:
//
// 0 -> Could not open file for writing
//
// 9 -> Success.
//

$fileLocation = "../../users/".$_POST['uname2']."/playTemp/move.txt";
$myfile = fopen($fileLocation, "w") or die("0");
fwrite($myfile, $_POST['move-digit']."\n".$_POST['x1']."\n".$_POST['y1']."\n".$_POST['x2']."\n".$_POST['y2']."\n");
fclose($myfile);

die("9");

?>