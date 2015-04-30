<?php

//
// Error Codes:
//
// 0 -> Could not open file
//
// 2 -> No user online
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
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

//Check for all online users
$date = mktime(); // Returns integral UNIX timestamp
$date = $date - 14400; // Reduces time by 4 hours

$sql = "UPDATE Users SET LastLogin='0000-00-00 00:00:00' WHERE LastLogin<'".date("Y-m-d H:i:s", $date)."'";
mysqli_query($conn, $sql) or die("6");

$sql = "SELECT UserName FROM Users WHERE LastLogin NOT IN ('0000-00-00 00:00:00')";
if($check = mysqli_query($conn, $sql)) {
	// Open the file
	$myfile = fopen("../../users/onlineUsers.txt", "w") or die("0");
	
	//Check for emptiness
	$check2 = mysqli_num_rows($check);
	if ($check2 == 0) {
		die("2");
	}

	while($info = mysqli_fetch_array( $check )) {
		//Write to file
		//echo $info['UserName']."<br />";
		fwrite($myfile, $info['UserName']."\n");
	}
	fclose($myfile);
}
else {
	die("6");
}

die("9");
?>