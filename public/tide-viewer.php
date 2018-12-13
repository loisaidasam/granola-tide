<?php

/**
 * Proxy between ESP8266 and NOAA's API for parsing/simplifying response
 *
 * Also helpful in the event that NOAA changes their API
 */

header("Content-Type: text/plain"); 

function get_data($product) {
    $base_url = "https://tidesandcurrents.noaa.gov/api/datagetter";
    $date = "latest";
    $station = "8418150";
    // $product = "water_level";
    $datum = "mllw";
    $units = "english";
    $time_zone = "lst_ldt";
    $format = "json";
    $application = "tide-viewer-v0.1-granola";

    $url = "$base_url?date=$date&station=$station&product=$product&datum=$datum&units=$units&time_zone=$time_zone&format=$format&application=$application";

    $contents = file_get_contents($url);

    $data = json_decode($contents);

    // print_r($data);
    /*
    water_level:

        stdClass Object
        (
            [metadata] => stdClass Object
                (
                    [id] => 8418150
                    [name] => Portland
                    [lat] => 43.6567
                    [lon] => -70.2467
                )
            [data] => Array
                (
                    [0] => stdClass Object
                        (
                            [t] => 2018-12-12 23:00
                            [v] => 2.764
                            [s] => 0.059
                            [f] => 0,0,0,0
                            [q] => p
                        )
                )
        )

    water_temperature:

        stdClass Object
        (
            [metadata] => stdClass Object
                (
                    [id] => 8418150
                    [name] => Portland
                    [lat] => 43.6567
                    [lon] => -70.2467
                )
            [data] => Array
                (
                    [0] => stdClass Object
                        (
                            [t] => 2018-12-12 23:06
                            [v] => 37.9
                            [f] => 0,0,0
                        )
                )
        )
    */
    return array($data->data[0]->t, $data->data[0]->v);
}

$data = get_data("water_level");
$timestamp1 = $data[0];
$water_level = $data[1];

$data = get_data("water_temperature");
$timestamp2 = $data[0];
$water_temperature = $data[1];

$result = "$timestamp1,$water_level,$timestamp2,$water_temperature\n";
print($result);
