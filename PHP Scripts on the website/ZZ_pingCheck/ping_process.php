<?php

//
// Error Codes:
//
// 0 -> Could not open file
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
    //die("5");
}

$date = mktime(); // Returns integral UNIX timestamp
$date = $date - 10; // Reduces time by 4 seconds
$sqll = "UPDATE Users SET LastPing='0000-00-00 00:00:00' WHERE LastPing<'".date("Y-m-d H:i:s", $date)."'";

mysqli_query($conn, $sqll); //or die("6");

// The SQL Statement
$sql = "SELECT * FROM Users WHERE Username='".$_POST['uname']."'";

if ($check1 = mysqli_query($conn, $sql)) {
	$fileName = "../../users/".$_POST['uname']."/pingReport.txt";
	//$myfile = fopen($fileName, 'w') or die("0");
	
	$info = mysqli_fetch_array( $check1 );
	if($info['LastPing'] !=  "0000-00-00 00:00:00") {
		// User is not online
		//fwrite($myfile, "YES");
		die("YES");
	}
	else {
		// User is online
		//fwrite($myfile, "NO");
		die("NO");
	}
	//fclose($myfile);
}
else {
    //die("6");
}

mysqli_close($conn);
die("0");

?>