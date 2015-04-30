<?php

//
// Error Codes:
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
//
// 7 -> Email ID not registered
// 
// 9 -> Success.
//


class X {
public static function deleteDir($dirPath) {
    if (! is_dir($dirPath)) {
        throw new InvalidArgumentException("$dirPath must be a directory");
    }
    if (substr($dirPath, strlen($dirPath) - 1, 1) != '/') {
        $dirPath .= '/';
    }
    $files = glob($dirPath . '*', GLOB_MARK);
    foreach ($files as $file) {
        if (is_dir($file)) {
            self::deleteDir($file);
        } else {
            unlink($file);
        }
    }
    rmdir($dirPath);
}
}


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

// Check if email ID exists
$sql = "SELECT * FROM Users WHERE Email='".$_POST['emai']."'";
if($check = mysqli_query($conn, $sql)) {
	$check2 = mysqli_num_rows($check);
	if ($check2 == 0) {
		die("7"); // Email ID not registered
	}
	$info = mysqli_fetch_array($check);
	$uname = $info['UserName'];
}

// Assuming everything is fine at this stage:
// The SQL Statement
$sql = "DELETE FROM Users WHERE Email='".$_POST['emai']."'";

if (mysqli_query($conn, $sql)) {
	$dirPath = "../../users/".$uname;
	
	// Delete the user's folder
    	X::deleteDir($dirPath);
    	
    	// Send confirmation email
    	$to = $_POST['emai'];
	$subject = "OCTPP Un-Registration";
	$message = "
	<html>
		<head>
			<title>OCTPP Un-Registration</title>
		</head>
		<body>
			<h1>You have been un-registered from OCTPP.</h1>
			<p>We\'re sorry to see you go.</p>
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