<?php

//
// Error Codes:
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 7 -> Username does not exist
// 
// 9 -> Success.
//

$servername = "localhost";
$username = "sqlUser";
$password = "Chess123";
$dbname = "TheData";

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname);

// Check connection
if (!$conn) {
    die("5");
}
$sqll = "SELECT UserName, Password FROM Users WHERE UserName = '".$_POST['uname']."'";
if($check = mysqli_query($conn, $sqll)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 == 0) {
		die("7");
	}

	$sql = "UPDATE Users SET LastLogin='".date("Y-m-d H:i:s")."' WHERE Username='".$_POST['uname']."'";
	if (!mysqli_query($conn, $sql)) {
  		die("6") ; //Other random error
	}
}
else {
	die("6");
}


mysqli_close($conn);
die("9");

?>