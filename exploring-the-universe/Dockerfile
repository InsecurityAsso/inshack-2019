# build image from debian buster (python 3.6+ required)
FROM debian:buster

RUN apt update && apt upgrade -y && \
    apt install -y python3 \
                   python3-pip

EXPOSE 8080

RUN mkdir -p /opt
WORKDIR /opt
# copy all files recursively from server-files/ to /opt/
ADD ./server-files/ /opt/
# install app requirements
RUN pip3 install -r requirements.txt
# start sesame service
CMD python3 /opt/server.py
