<?php

//
// Error Codes:
//
// 0 -> ERROR
//
// NO -> Move not made
//

// $fileLocation = "../../users/".$_POST['uname1']."/playTemp/move_made.txt";
// $myfile = fopen($fileLocation, 'w') or die("0");

$servername = "localhost";
$username = "sqlUser";
$password = "Chess123";
$dbname = "TheData";

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname) or die("0");

/*
$myfile = "../../users/".$_POST['uname2']."/playTemp/move.txt";
$file = fopen($myfile, "r") or die("0");
fclose($file);
*/


$sqll = "SELECT LastMove FROM Users WHERE UserName='".$_POST['uname2']."' AND LastMove!='0000-00-00 00:00:00'";
if($check = mysqli_query($conn, $sqll)) {
	$check2 = mysqli_num_rows($check);
	if($check2 == 0) {
		die("NO");
	}
	else {
		/*$myfile = "../../users/".$_POST['uname2']."/playTemp/move.txt";
		$file = fopen($myfile, "r") or die("0");
		fclose($file); */
		$move = file_get_contents("../../users/".$_POST['uname1']."/playTemp/move.txt");
		die($move);
	}
}
else {
	die("0");
}

mysqli_close($conn);
die("0");

?>