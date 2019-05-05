#!/usr/bin/env bash

. scripts/common.sh
exec &> ${COMMON_LOG_DIR}/start-noise.log

${COMMON_VENV}/bin/passthru-noise
