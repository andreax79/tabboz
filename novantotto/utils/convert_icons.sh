#!/bin/bash
if [ -z "$RESOURCES_DIR" ]; then
    echo "Error: RESOURCES_DIR environment variable is not set."
    exit 1
fi

cd "${RESOURCES_DIR}/icons"
for file in *.ico; do
    if [ -f "$file" ]; then
        n="${file%.*}"
        target="${n}".gif
        convert "${file}" "${target}"
    fi
done
