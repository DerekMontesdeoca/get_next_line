#!/usr/bin/bash

DEST_DIR="$1"
SOURCE_DIR="$(dirname "$(realpath "$0")")"

mkdir -p "$DEST_DIR"
rsync -azv --delete "$SOURCE_DIR/"*.{h,c} "$DEST_DIR"
