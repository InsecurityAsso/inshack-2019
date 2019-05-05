FROM python:3.7-alpine

RUN apk update && apk upgrade && \
    adduser -D sandbox && \
    mkdir -p /sandbox/

# Modify here to put yours files into container's /sandbox/
ADD server-files/jail.py /sandbox/
RUN cd /sandbox && python3 -m compileall jail.py -b && rm /sandbox/jail.py

RUN chown -R sandbox:sandbox /sandbox

WORKDIR /sandbox
USER sandbox

ARG FLAG=INSA{ERROR_MISSING_FLAG___REPORT_THIS_TO_CTF_ADMINS}
ENV 0f4d0db3668dd58cabb9eb409657eaa8 $FLAG
# Setup your command
CMD python3 jail.pyc
