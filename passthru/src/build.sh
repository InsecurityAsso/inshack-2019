#!/usr/bin/env bash

set -e

. scripts/common.sh

ARCH=../public-files/passthru.zip
DELAY=5
REQUESTBIN_ID=zk2s2ezk

print "- Setting up venv & installing passthru..."
python3 -m venv ${COMMON_VENV}
${COMMON_VENV}/bin/pip install --upgrade .

print "- Generating payload..."
scripts/generate-payload.sh

print "- Starting mitmproxy..."
scripts/start-mitmproxy.sh & # killed when script exits
print "- Waiting 5 seconds for mitmweb to start..."
sleep 5

print "- Starting tcpdump..."
scripts/start-tcpdump.sh & # killed when script exists
print "- Waiting 30 seconds for tcpdump to start..."
sleep 30

print "- Starting passthru-noise..."
scripts/start-noise.sh & # killed when script exists
print "- Waiting 30 seconds for passthru-noise to start and send some packets..."
sleep 30

print "- Running passthru with requestbin id: ${REQUESTBIN_ID}"
${COMMON_VENV}/bin/passthru ${COMMON_PAYLOAD} ${REQUESTBIN_ID}

print "- Waiting 30 seconds for passthru-noise to send more packets before exiting..."
sleep 30

print "- Murdering some processes..."
printf "\x1b[31m"
cat scripts/death.ascii
printf "\x1b[0m"
print "-- Killing remaining passthru-noise..."
sudo kill -9 $(pgrep passthru-noise)
print "-- Killing remaining tcpdump..."
sudo kill -9 $(pgrep tcpdump)
print "-- Killing remaining mitmweb..."
sudo kill -9 $(pgrep mitmweb)

print "- Exporting challenge data..."
zip -j ${ARCH} ${COMMON_CAPTURE} ${COMMON_SSLKEY_LOG}

print "- Exiting."
exit 0
