<?php

//Error codes for this is pointless.

$num = filesize("../../users/".$_POST['uname']."/");
clearstatcache();

die("".$num);

?>