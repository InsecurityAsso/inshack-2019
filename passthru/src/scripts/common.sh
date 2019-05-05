COMMON_BUILD_DIR=build-tmp
mkdir -p ${COMMON_BUILD_DIR}
COMMON_VENV=${COMMON_BUILD_DIR}/venv
COMMON_LOG_DIR=${COMMON_BUILD_DIR}/logs
mkdir -p ${COMMON_LOG_DIR}
COMMON_PAYLOAD=${COMMON_BUILD_DIR}/payload.bin
COMMON_CAPTURE=${COMMON_BUILD_DIR}/capture.pcap
COMMON_SSLKEY_LOG=${PWD}/${COMMON_BUILD_DIR}/sslkey.log

function print {
    printf "\x1b[34m${1}\x1b[0m\n"
}
