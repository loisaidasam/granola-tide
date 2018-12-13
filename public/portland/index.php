<?php

header("Content-Type: text/plain"); 

require_once('../../tide-viewer.php');

$result = get_station_data("8418150");

print($result);
