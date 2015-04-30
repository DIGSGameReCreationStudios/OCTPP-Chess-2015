<?php

//
// Error Codes:
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 7 -> Unregistered email ID
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


$sqll = "SELECT UserName, Password, Email FROM Users WHERE Email = '".$_POST['emai']."'";
if($check = mysqli_query($conn, $sqll)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 == 0) {
		die("7");
	}
	
	// Confiramtion
	echo "Email sent.";
	
	$info = mysqli_fetch_array( $check );
		
	// Send confirmation email
    	$to = $info['Email'];
	$subject = "OCTPP Account Recovery";
	$message = "
	<html>
		<head>
			<title>Account Recovery</title>
		</head>
		<body>
			<h1>Account Recovery</h1>
			<p>We received a request to email you your credentials again. Here they are.</p>
			<table border=0>
				<tr>
					<td><b>Username:</b></td>
					<td>".$info['UserName']."</td>
				</tr>
				<tr>
					<td><b>Password:</b></td>
					<td>".$info['Password']."</td>
				</tr>
			</table>
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