#!/usr/bin/bash

DEST_DIR="$1"
SOURCE_DIR="$(dirname "$(realpath "$0")")"

mkdir -p "$DEST_DIR"
rsync -azv --delete \
    --files-from=<(find "$SOURCE_DIR" -maxdepth 1 -mindepth 1 \
        -regextype "awk" -regex '.*\.(h|c)' -exec basename '{}' ';') \
    "$SOURCE_DIR/" \
    "$DEST_DIR"
