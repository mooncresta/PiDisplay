<?php
# Simple web page to accept GET's from IFTTT or any other webhook provider
# and post a message to a MQTT bus.
#
# Was originally intended to push messages from my Ring door bell to MQTT.
#
# Examples:
#
# GET http://localhost/ifttt-mqtt.php??thing=doorbell&time=now&kind=ring
#
# Can be used in a IFTTT recipe with a Ring doorbell.
#
# Requirements:
#
# Any vaguely new version of PHP, and the MQTT library from here:
# https://www.cloudmqtt.com/docs-php.html
#
require("phpMQTT.php");
$host = "hostname";
$port = "1883";
$username = "username";
$password = "password";
$topic = "ring";
$file = '/tmp/ifttt.txt';
$arr = get_defined_vars();
$data = json_encode($arr,JSON_PRETTY_PRINT);
$thing = $_GET['thing'];
$time = $_GET['time'];
$kind = $_GET['kind'];
# Write it out all the vars to a log file just
# for debugging and checking what is going on.
$current = file_get_contents($file);
$current .= $data . "\n";
file_put_contents($file, $current);
# If its the doorbell (only thing supported right now) then
# publish a simple JSON message to MQTT
if ( $thing == "doorbell" ){
    $message = [ 
                "kind" => $kind,
                "time" => $time
                ];
    $mqtt = new phpMQTT($host, $port, "iftt.php".rand());
    if ($mqtt->connect(true, NULL, $username, $password)) {
        $mqtt->publish($topic, json_encode($message), 0, true);
        $mqtt->close();
    }
}
?>
<html>
<body>
<pre>
<?php 
# print out all returned data for debugging
print $data; 
?>
</pre>
</body>
</html>
