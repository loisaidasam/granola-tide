# tide-viewer

Supporting software for ESP8266 Tide Viewer project

## Project overview

Using LEDs and OLED screen, show current tide and water temp.

## Project design

- 8 LEDs connected to digital GPIO pins of a NodeMCU (ESP8266) w/ 330Ω resistors

LED Layout:

```
* > 10 ft
* 10 ft
* 8 ft
* 6 ft
* 4 ft
* 2 ft
* 0 ft
* < 0 ft
##### (OLED w/ timestamp/water level/water temp)
```

## Software

- [tide-viewer](https://github.com/loisaidasam/tide-viewer/blob/master/tide-viewer)
  - bash script for analyzing potential data
- [tide-viewer.php](https://github.com/loisaidasam/tide-viewer/blob/master/tide-viewer.php)
  - PHP web proxy helper functions
- [public/portland/index.php](https://github.com/loisaidasam/tide-viewer/blob/master/public/portland/index.php)
  - PHP web proxy for Portland, ME station

## References:

- NOAA
    - https://tidesandcurrents.noaa.gov/api/
    - https://tidesandcurrents.noaa.gov/waterlevels.html?id=8418150
    - https://tidesandcurrents.noaa.gov/water_level_info.html
    - https://tidesandcurrents.noaa.gov/datum_options.html
    - https://tidesandcurrents.noaa.gov/datums.html?id=8418150
    - https://tidesandcurrents.noaa.gov/map/?id=8725520
- Other tide data
    - https://me.usharbors.com/monthly-tides/Maine-Southern%20Coast/Portland%20Harbor
    - https://www.pressherald.com/weather/?t=04103
- Stations
    - Portland: `8418150`
    - Ft. Myers: `8725520`
- OLED:
    - https://www.hackster.io/tarantula3/i2c-oled-display-using-arduino-nodemcu-7682e8
