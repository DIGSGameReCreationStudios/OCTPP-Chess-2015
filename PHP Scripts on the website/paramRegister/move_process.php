<?php

//
// Error Codes:
//
// 0 -> Could not open file for writing
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 9 -> Success.
//

//
// Parameter Codes:
// 
// 1 -> Queen
// 2 -> Rook
// 3 -> Bishop
// 4 -> Knight
//

$fileLocation = "../../users/".$_POST['uname2']."/playTemp/param.txt";
$myfile = fopen($fileLocation, 'w') or die("0");
fwrite($myfile, $_POST['param']);
fclose($myfile);

die("9");

?>