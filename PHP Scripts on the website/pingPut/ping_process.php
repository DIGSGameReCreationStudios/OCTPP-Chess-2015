<?php

//
// Error Codes:
//
// 0 -> File could not be opened
// 
// 9 -> Success.
//

$file = fopen("../../users/".$_POST['uname']."/playTemp/ping.txt", "w") or die("0");
fwrite($file, $_POST['ping-digit']."\n");
fclose($file);

die("9");

?>