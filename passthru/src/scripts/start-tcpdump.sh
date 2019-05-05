#!/usr/bin/env bash

. scripts/common.sh
exec &> ${COMMON_LOG_DIR}/start-tcpdump.log

if [ -f ${COMMON_CAPTURE} ]; then
    print "- Removing previous capture file"
    rm ${COMMON_CAPTURE}
fi
print "- Starting tcpdump..."
sudo tcpdump -n -v -ilo -w ${COMMON_CAPTURE}
