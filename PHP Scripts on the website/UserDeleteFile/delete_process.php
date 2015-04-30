<?php

// This does not update index.txt. It's your responsibility to do that.


$filePath = "../../users/".$_POST['uname']."/".$_POST['mode']."/".$_POST['filee'];
$indexFile = "../../users/".$_POST['uname']."/".$_POST['mode']."/index.txt";

// Update index.txt

/*
$fileContents = file_get_contents($indexFile);
$output = str_replace($_POST['filee'] ."\r\n", "", $fileContents ."\r\n");
file_put_contents($indexFile,$output);
*/

$contents = file_get_contents($indexFile);
$contents = str_replace($_POST['filee']."\n", '', $contents);
file_put_contents($indexFile, $contents);

unlink($filePath);

die("9");

?>