#!/usr/bin/env bash

. scripts/common.sh
exec &> ${COMMON_LOG_DIR}/start-mitmproxy.log

export MITMPROXY_SSLKEYLOGFILE="${COMMON_SSLKEY_LOG}"

if [ -f ${MITMPROXY_SSLKEYLOGFILE} ]; then
    print "- Removing previous sslkey.log"
    rm ${MITMPROXY_SSLKEYLOGFILE}
fi
print "- Starting mitmproxy..."
${COMMON_VENV}/bin/mitmweb --no-web-open-browser --listen-port 8080 --proxyauth hello:world
