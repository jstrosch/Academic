<?php

$get_headers = apache_request_headers();

echo $_SERVER['REQUEST_METHOD'] . " " . $_SERVER['REQUEST_URI'] . " " . $_SERVER['SERVER_PROTOCOL'] . "<br><hr>";

echo "<table width='100%' style='table-layout:fixed;'>";
foreach ($get_headers as $header => $value) {
    echo "<tr><td style='width:15%;'><b>$header:</b></td><td style='width:85%;word-wrap:break-word'>$value</td></tr>";
}
echo "</table>";

echo "<br><br>Your IP address is: " . $_SERVER['REMOTE_ADDR'];

?>
