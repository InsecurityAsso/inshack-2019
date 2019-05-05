FROM ubuntu:16.04

RUN apt-get update && apt-get upgrade -y
RUN apt-get install libc6-dev-i386 clang make gdb wget -y
RUN useradd sandbox
RUN mkdir -p /sandbox

ADD server-files/main.c /sandbox
ADD server-files/Makefile /sandbox
ADD server-files/flag.txt /sandbox
RUN chown root:sandbox /sandbox/flag.txt
RUN chmod 440 /sandbox/flag.txt

WORKDIR /sandbox

RUN make && rm main.c Makefile
RUN chown root:sandbox intergover

RUN cat flag.txt

USER sandbox
CMD /sandbox/intergover
