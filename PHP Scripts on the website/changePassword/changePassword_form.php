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

$_POST['passwd'] = stripslashes($_POST['passwd']);

// The SQL Statement

$sqll = "SELECT UserName, Password, Email FROM Users WHERE UserName = '".$_POST['uname']."'";
if($check = mysqli_query($conn, $sqll)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 == 0) {
		die("7");
	}

		$_POST['newpwd'] = stripslashes($_POST['newpwd']);
		$sql = "UPDATE Users SET Password='".$_POST['newpwd']."' WHERE Username='".$_POST['uname']."'";

		if (mysqli_query($conn, $sql)) {    	
			// Send confirmation email
 			$to = $info['Email'];
			$subject = "OCTPP Password Change";
			$message = "
				<html>
					<head>
						<title>Password Change Successful</title>
					</head>
					<body>
						<h1>Password Change Successful</h1>
						<p>We received a request to change your OCTPP password. Here are your new credentials:</p>
						<table border=0>
							<tr>
								<td><b>Username:</b></td>
								<td>".$_POST['uname']."</td>
							</tr>
							<tr>
								<td><b>Password:</b></td>
								<td>".$_POST['newpwd']."</td>
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
}

mysqli_close($conn);

die("9");
?>