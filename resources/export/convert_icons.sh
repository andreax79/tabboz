#!/usr/bin/env bash
cd ../icons
for file in *.ico; do
    if [ -f "$file" ]; then
        n="${file%.*}"
        target="${n}".gif
        convert "${file}" "${target}"
    fi
done
