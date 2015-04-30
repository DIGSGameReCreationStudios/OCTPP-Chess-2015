<?php

//
// Error Codes:
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 9 -> Success.
//

$fileLocation1 = "../../users/".$_POST['uname1']."/playTemp/move.txt";
// $fileLocation2 = "../../users/".$_POST['uname1']."/playTemp/move_made.txt";
unlink($fileLocation1);
// unlink($fileLocation2);

$servername = "localhost";
$username = "sqlUser";
$password = "Chess123";
$dbname = "TheData";

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname) or die("5");

$sqll = "UPDATE Users SET LastMove='0000-00-00 00:00:00' WHERE UserName='".$_POST['uname2']."'";
if(!mysqli_query($conn, $sqll)) {
	die("6");
}

mysqli_close($conn);

die("9");

?>