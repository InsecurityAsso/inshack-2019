FROM ubuntu:12.04

RUN apt-get update && apt-get upgrade -y
RUN apt-get install libc6-dev-i386 clang make -y
RUN useradd sandbox

RUN mkdir -p /sandbox
WORKDIR /sandbox

ADD server-files/main.c /sandbox
ADD server-files/Makefile /sandbox
ADD server-files/flag.txt /sandbox
RUN chown root:sandbox /sandbox/flag.txt
RUN chmod 440 /sandbox/flag.txt

RUN make && rm Makefile main.c

RUN cat flag.txt

USER sandbox
CMD ./ropberry
