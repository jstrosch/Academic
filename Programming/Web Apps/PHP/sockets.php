
<?php

/*  

This script was written to open a TCP socket to a server and waiting for the response,
the response tells you how many words to return, the server waits for this script to send
the response (ie the correct number of words)

*/

error_reporting(E_ALL);

echo "** TCP/IP Connection **\n";

$service_port = '2211';
//getservbyname('www', 'tcp');

/* Get the IP address for the target host. */
$address = gethostbyname('www.google.com');

/* Create a TCP/IP socket. */
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "[ERROR] socket_create() failed: " . socket_strerror(socket_last_error()) . "\n";
} else {
    echo "[INFO] Connection Successful.\n";
}

echo "[INFO] Attempting to connect to '$address' on port '$service_port'...";
$result = socket_connect($socket, $address, $service_port);

if ($result === false) {
    echo "[ERROR] socket_connect() failed: $result\n" . socket_strerror(socket_last_error($socket)) . "\n";
} 

while ($out = socket_read($socket, 2048)) {

    echo "[RECEIVED] " . $out;

	preg_match_all('/Return the (first|last) (\d{1,}) words: (.{1,})\n{0,1}/i', $out, $matches, PREG_OFFSET_CAPTURE);
	
	$firstLast = "";
	$sentence = "";

	$wordIndex = $matches[2][1][0];

	$firstLast = $matches[1][1][0];

	$sentence = $matches[3][1][0];

	$response = "";

	if($firstLast == 'first'){

		$explode = explode(" ", $sentence);

		for($i = 0; $i < $wordIndex; $i++)
		{
			$response .= $explode[$i] . ' ';
		}

	} else {

		$explode = explode(" ", $sentence);

		for($i = count($explode) - $wordIndex; $i < count($explode); $i++)
		{
			$response .= $explode[$i] . ' ';
		}
	}

	if(strlen($response) > 0)
	{
		$response = substr($response,0, strlen($response) - 1 );

		$response .= "\n";

		socket_write($socket, $response);
	}
}

echo "[INFO] Closing socket...";
socket_close($socket);
echo "[INFO] OK.\n\n";
?>
