<?php

//
// Error Codes:
//
// 0 -> Could not open file for overwriting
//
// 5 -> Connection to database could not be established
// 6 -> SQL Statement could not be run
// 
// 7 -> Username does not exist
// 8 -> Wrong Password
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
    //rmdir($dirPath);
    //Otherwise, it would delete the parameter directory as well!
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
$sqll = "SELECT UserName, Password FROM Users WHERE UserName = '".$_POST['uname']."'";
if($check = mysqli_query($conn, $sqll)) {
	$check2 = mysqli_num_rows($check);

	if ($check2 == 0) {
		die("7");
	}

	while($info = mysqli_fetch_array( $check )) {
		$_POST['passwd'] = stripslashes($_POST['passwd']);
 		$info['Password'] = stripslashes($info['Password']);

 		if ($_POST['passwd'] != $info['Password']) {
 			die("8"); 
 		}
		else {
			// Update last login
			// The SQL Statement
			$sql = "UPDATE Users SET LastLogin='".date("Y-m-d H:i:s")."', LastPing='0000-00-00 00:00:00', LastMove='0000-00-00 00:00:00' WHERE Username='".$_POST['uname']."'";

			if (!mysqli_query($conn, $sql)) {
    				die("6") ; //Other random error
			}
		}
	}
}
else {
	die("6");
}

// Reset stuff for online purposes
$file = fopen("../../users/".$_POST['uname']."/playTemp/move.txt", "w") or die("0");
fclose($file);
$file = fopen("../../users/".$_POST['uname']."/playTemp/ping.txt", "w") or die("0");
fclose($file);

X::deleteDir("../../users/".$_POST['uname']."/requests");
X::deleteDir("../../users/".$_POST['uname']."/accepts");

mysqli_close($conn);
die("9");

?>