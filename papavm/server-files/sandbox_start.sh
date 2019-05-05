#!/bin/bash

read -p "What's the magic string you've found?" flag
echo "launch" > /tmp/inp.ut
echo $flag >> /tmp/inp.ut
cat /tmp/inp.ut | ./papavm
if [ $? -ne 8 ]; then
    echo "Try again."
else
    cat flag.txt
fi

