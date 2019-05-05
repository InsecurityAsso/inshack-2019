#!/bin/sh
name="ropberry_sandbox-$(pwgen 20 1)"

timeout --foreground --kill-after=10 480 \
        docker run \
        --rm -i \
        --cpus=1 \
        --name=${name} \
        --ulimit nproc=1024:1024 \
        --ulimit fsize=1000000:1000000 \
        --ulimit nofile=1024:2048 \
        --ulimit nice=1 \
        registry-chal.infra.insecurity-insa.fr/insecurity/ropberry-sandbox

docker kill ${name} >/dev/null 2>&1 || exit 0
