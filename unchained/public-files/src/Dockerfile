FROM debian:9

RUN apt update && apt upgrade -y
RUN apt install git nginx uwsgi uwsgi-plugin-python3 python3-pip -y
RUN git clone https://github.com/django/unchained >/dev/null 2>&1 || true

ADD requirements.txt /tmp
RUN pip3 install -r /tmp/requirements.txt && rm /tmp/requirements.txt
RUN useradd me && mkdir -p /srv/app && chown -R me:me /srv/app

ADD unchained/ /srv/app
ADD nginx.conf /etc/nginx/sites-enabled/default
ADD uwsgi.ini /etc/uwsgi/apps-enabled/uwsgi.ini

ARG FLAG
RUN echo $FLAG > /tmp/flag

WORKDIR /srv/app

EXPOSE 8000

CMD service uwsgi start ; service nginx start && tail -f /tmp/unchained.log

