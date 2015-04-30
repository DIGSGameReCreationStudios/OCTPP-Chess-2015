<?php

//
// Error Codes:
//
// 0 -> Could not open file
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
//
// 7 -> Username already exists
// 8 -> Email ID already registered
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

$_POST['passwd'] = stripslashes($_POST['passwd']);

// Check if username exists
$sql = "SELECT Username FROM Users WHERE Username='".$_POST['uname']."'";
if($check = mysqli_query($conn, $sql)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 != 0) {
		die("7"); // Username already exists
	}
}
else {
	die("6");
}

// Check if email ID is already in use
$sql = "SELECT Username FROM Users WHERE Email='".$_POST['emai']."'";
if($check = mysqli_query($conn, $sql)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 != 0) {
		die("8"); // Email ID already registered
	}
}

// Assuming everything is fine at this stage:
// The SQL Statement
$sql = "INSERT INTO Users (Username, Password, Email) VALUES ('".$_POST['uname']."', '".$_POST['passwd']."', '".$_POST['emai']."')";

if (mysqli_query($conn, $sql)) {
	$fileLocation = "../../users/".$_POST['uname'];
    	
    	// Create the user's folder
	mkdir($fileLocation);
	mkdir($fileLocation."/requests");
	mkdir($fileLocation."/accepts");
	mkdir($fileLocation."/savedGames");
	mkdir($fileLocation."/completedGames");
	mkdir($fileLocation."/playTemp");
	
	// Create user files
	$myfile = fopen($fileLocation."/savedGames/index.txt", "w") or die("0");
	fclose($myfile);
	$myfile = fopen($fileLocation."/completedGames/index.txt", "w");
	fclose($myfile) or die("0");
	
	$myfile = fopen($fileLocation."/wins.txt", "w");
	fwrite($myfile, "0");
	fclose($myfile) or die("0");
	$myfile = fopen($fileLocation."/losses.txt", "w");
	fwrite($myfile, "0");
	fclose($myfile) or die("0");
	
	$myfile = fopen($fileLocation."/draws.txt", "w");
	fwrite($myfile, "0");
	fclose($myfile) or die("0");
	
	$myfile = fopen($fileLocation."/playTemp/move.txt", "w") or die("0");
	fclose($myfile) or die("0");
	$myfile = fopen($fileLocation."/playTemp/ping.txt", "w") or die("0");
	fclose($myfile) or die("0");
	
    	
    	// Send confirmation email
    	$to = $_POST['emai'];
	$subject = "OCTPP Registration Successful!";
	$message = "
	<html>
		<head>
			<title>Registration Successful!</title>
		</head>
		<body>
			<h1>Thanks for registering!</h1>
			<p>You now have access to one of the greatest free chess programs ever created!</p>
			<table border=0>
				<tr>
					<td><b>Username:</b></td>
					<td>".$_POST['uname']."</td>
				</tr>
				<tr>
					<td><b>Password:</b></td>
					<td>".$_POST['passwd']."</td>
				</tr>
			</table>
			<br />
			<p>With warm regards,<br />The OCTPP Team
		</body>
	</html>
	";

	// Always set content-type when sending HTML email
	$headers = "MIME-Version: 1.0" . "\r\n";
	$headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
	// More headers
	$headers .= 'From: <do-not-reply@octpp.heliohost.org>' . "\r\n";
	
	mail($to,$subject,$message,$headers);
}
	
else {
    die("6");
}

mysqli_close($conn);
die("9");
?>