# build image from debian buster (python 3.6+ required)
FROM ubuntu:19.04

RUN apt update && apt upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    python3 python3-pip \
    tshark tcpdump \
    iproute2

RUN mkdir -p /opt

WORKDIR /opt

# copy all files recursively from server-files/ to /opt/
ADD ./server-files/ /opt/

# install app requirements
RUN pip3 install -r requirements.txt

# set flag as an env variable within docker
ENV SESAME_FLAG={{SESAME_FLAG}}

# expose following ports with TCP protocol
EXPOSE 10000:10000/tcp
EXPOSE 10010:10010/tcp
EXPOSE 10020:10020/tcp
EXPOSE 10040:10040/tcp
EXPOSE 10060:10060/tcp
EXPOSE 10080:10080/tcp
EXPOSE 10090:10090/tcp

# expose door port
EXPOSE 20000:20000/tcp

# expose following ports with UDP protocol
EXPOSE 10000:10000/udp
EXPOSE 10010:10010/udp
EXPOSE 10020:10020/udp
EXPOSE 10040:10040/udp
EXPOSE 10060:10060/udp
EXPOSE 10080:10080/udp
EXPOSE 10090:10090/udp

# start sesame service
CMD python3 /opt/sesame.py eth0 /opt/sesame.json
