<?php


$filePath = "../../users/".$_POST['uname']."/".$_POST['mode']."/".$_POST['filee'];
$indexFile = "../../users/".$_POST['uname']."/".$_POST['mode']."/index.txt";

// Update index.txt

$file = $_POST['filee']."\n";
file_put_contents($indexFile, $file, FILE_APPEND);

$myfilee = fopen($filePath, "w");
fwrite($myfilee, $_POST['contents']);
fclose($myfilee);

die("9");

?>