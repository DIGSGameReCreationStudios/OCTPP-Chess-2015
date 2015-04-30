<?php

//
// Error Codes:
//
// 0 -> Report File could not be opened
// 1 -> Request File could not be deleted
//
// 9 -> Success.
//

$filename = "../../users/".$_POST['uname']."/draws.txt";
$count = @file_get_contents($filename);
$count++;
@file_put_contents($filename, $count);
die("9");
?>