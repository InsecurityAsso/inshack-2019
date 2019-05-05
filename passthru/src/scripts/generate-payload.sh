#!/usr/bin/env bash

. scripts/common.sh

RNDSRC=/dev/urandom
MKCTF_FILE=../.mkctf.yml

print "- Generating payload..."
head -c 32 ${RNDSRC} > ${COMMON_PAYLOAD}
grep flag ${MKCTF_FILE} | cut -d' ' -f2 >> ${COMMON_PAYLOAD}
head -c 32 ${RNDSRC} >> ${COMMON_PAYLOAD}
print "- Generated payload (${COMMON_PAYLOAD}):"
cat ${COMMON_PAYLOAD} | hd
