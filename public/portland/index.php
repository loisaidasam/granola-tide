<?php

header("Content-Type: text/plain"); 

require_once('../../tide-viewer.php');

authenticate();

print get_station_data("8418150");
