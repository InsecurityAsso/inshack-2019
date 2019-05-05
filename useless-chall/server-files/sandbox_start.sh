#!/bin/bash

read -p "What's the magic string you've found?" flag
./hash $flag
if [ $? -ne 0 ]; then
    echo "Try again."
else
    cat flag.txt
fi

