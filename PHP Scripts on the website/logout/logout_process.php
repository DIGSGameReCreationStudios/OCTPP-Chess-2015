<?php

//
// Error Codes:
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 7 -> Username does not exist
// 8 -> Wrong Password
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

$sql = "SELECT * FROM Users WHERE Username='".$_POST['uname']."'";
if ($check = mysqli_query($conn, $sql)) {
    $check2 = mysqli_num_rows($check);
    if($check2 == 0) die("7"); //Username doesn't exist.
}
else {
	die("6");
}

// The SQL Statement
$sql = "UPDATE Users
SET LastLogin='0000-00-00 00:00:00' WHERE Username='".$_POST['uname']."'";

if (!mysqli_query($conn, $sql)) {
    die("6");
}

mysqli_close($conn);

$file = fopen("../../users/".$_POST['uname']."/playTemp/move.txt", "w");
fclose($file);
$file = fopen("../../users/".$_POST['uname']."/playTemp/ping.txt", "w");
fclose($file);

die("9");
?>