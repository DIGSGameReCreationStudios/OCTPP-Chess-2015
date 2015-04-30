<?php

//
// Error Codes:
//
// 0 -> Report File could not be opened
//
// 9 -> Success.
//

$filename = "../../users/".$_POST['uname']."/requests/request.txt";
$reportName = "../../users/".$_POST['uname']."/requests/report.txt";
$reportFile = fopen($reportName, 'w') or die("0");
if($myfile = fopen($filename, 'r')) {
	fwrite($reportFile, "YES");
	fclose($myfile);
}
else {
	fwrite($reportFile, "NO");
}
fclose($reportFile);
die("9");
?>