#!/usr/bin/env bash
cd ../bitmaps
echo '{ "data": [' > list.json
ls -b --quote-name *.png | sed '$!s/$/,/' >> list.json
echo ']}' >> list.json
