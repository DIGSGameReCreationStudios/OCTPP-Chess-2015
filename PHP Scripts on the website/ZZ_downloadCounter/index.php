<html>

<head>
	<title>Your download will begin shortly...</title>

<?php

$hit_count = @file_get_contents('counter.txt'); // read the hit count from file
$hit_count++; // increment the hit count by 1
@file_put_contents('counter.txt', $hit_count); // store the new hit count

?>

<meta http-equiv="refresh" content="2; URL=http://octpp.tk/download/octpp_setup.exe">
<script type="text/javascript">
	function redi() {
		window.location.href="http://octpp.tk/#support";
	}
</script>
</head>

<body onLoad="setTimeout('redi()', 4000);">
<h1>Your download will begin shortly...</h1>
</body>
</html>