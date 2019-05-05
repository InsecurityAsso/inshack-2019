FROM debian:buster

RUN apt update && apt upgrade -y && apt install -y python3-pip && pip3 install pycrypto
RUN useradd sandbox && \
    mkdir -p /sandbox/ 

# Modify here to put yours files into container's /sandbox/
ADD server-files/server.py /sandbox/
ADD server-files/flag.txt /sandbox/

WORKDIR /sandbox
USER sandbox

# Setup your command
CMD python3 server.py
