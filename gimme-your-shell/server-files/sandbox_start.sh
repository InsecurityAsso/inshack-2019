#!/bin/sh
name="hell_of_a_jail_sandbox-$(pwgen 20 1)"

timeout --foreground --kill-after=10 480 \
        docker run \
        --rm -i \
        --cpus=1 \
        --name=${name} \
        --ulimit nproc=1024:1024 \
        --ulimit fsize=10000:10000 \
        --ulimit nofile=1024:2048 \
        --ulimit nice=1 \
        registry-chal.infra.insecurity-insa.fr/insecurity/gimme-your-shell-sandbox

docker kill ${name} >/dev/null 2>&1 || exit 0
