FROM ubuntu:18.04

RUN apt-get update && apt-get upgrade -y
RUN useradd sandbox

RUN mkdir -p /sandbox
WORKDIR /sandbox

#ADD server-files/main.c /sandbox
#ADD server-files/Makefile /sandbox
ADD server-files/flag.txt /sandbox
ADD server-files/exec /sandbox
RUN chown root:sandbox /sandbox/flag.txt
RUN chmod 440 /sandbox/flag.txt

RUN cat flag.txt

USER sandbox
CMD ./exec
