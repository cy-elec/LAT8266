#!/bin/sh
rm LAT8266.zip
cd ..&& zip -r LAT8266/LAT8266.zip LAT8266/ -x *LAT8266.zip -x *.git* -x *TODO -x *build.sh
