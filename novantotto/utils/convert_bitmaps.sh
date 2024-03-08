#!/bin/bash
if [ -z "$RESOURCES_DIR" ]; then
    echo "Error: RESOURCES_DIR environment variable is not set."
    exit 1
fi

cd "${RESOURCES_DIR}/bitmaps"
for file in *.bmp; do
    if [ -f "$file" ]; then
        n="${file%.*}"
        target="${n}".png
        if [ "$n" -lt 1370 ]; then
            convert -define bmp:ignore-filesize "${file}" "${target}"
        elif [ "$n" -gt 1550 ]; then
            convert -define bmp:ignore-filesize "${file}" "${target}"
        else
            convert -define bmp:ignore-filesize -transparent yellow "${file}" "${target}"
        fi
    fi
done
