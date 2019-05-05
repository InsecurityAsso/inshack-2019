FROM ubuntu:16.04

RUN apt update && apt upgrade -y
RUN apt install libc6-dev open-cobol make wget vim bsdmainutils python -y
RUN useradd sandbox
RUN mkdir -p /sandbox

ADD server-files/lib.c /sandbox
ADD server-files/Makefile /sandbox
ADD public-files/Test.CBL /sandbox
ADD server-files/flag.txt /sandbox
RUN chown root:sandbox /sandbox/flag.txt
RUN chmod 440 /sandbox/flag.txt

WORKDIR /sandbox

RUN make && rm Makefile Test.CBL
RUN chown root:sandbox overcobol && chmod 550 overcobol

RUN cat flag.txt

USER sandbox
CMD ./overcobol
