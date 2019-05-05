FROM debian:stretch

RUN apt update && apt upgrade -y && \
    apt install -y python3 python3-pip

RUN pip3 install flask

EXPOSE 4321

RUN mkdir -p /opt

WORKDIR /opt

ADD ./server-files/ /opt/

CMD python3 /opt/server.py
