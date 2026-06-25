#!/bin/bash
# Create directory if it doesn't exist
mkdir -p zBack
# Copy files with numbered backups, checking if files exist
if ls *.c 1> /dev/null 2>&1; then
    cp --backup=t *.{c,cpp,h} zBack/
    echo "Backup completed."
else
    echo "No .c files found."
fi

