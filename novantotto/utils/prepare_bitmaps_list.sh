#!/usr/bin/env bash
if [ -z "$RESOURCES_DIR" ]; then
    echo "Error: RESOURCES_DIR environment variable is not set."
    exit 1
fi

cd "${RESOURCES_DIR}/bitmaps"
echo '{ "data": [' > list.json
ls -b --quote-name *.png | sed '$!s/$/,/' >> list.json
echo ']}' >> list.json
