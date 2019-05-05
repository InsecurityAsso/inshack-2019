FROM debian:buster

RUN apt update && apt upgrade -y && apt install -y python3-pip cron build-essential python3-dev chromium

ADD requirements.txt /tmp
RUN pip3 install -r /tmp/requirements.txt && rm /tmp/requirements.txt

RUN useradd chal && mkdir -p /srv/app && chown -R chal:chal /srv/app
RUN echo '* * * * * chal cd /srv/app && python3 get_urls.py > /tmp/log 2>&1' >> /etc/crontab && \
    echo >> /etc/crontab

WORKDIR /srv/app

ADD . .

CMD service cron start && su chal -c "uwsgi --socket 0.0.0.0:8080 --protocol=http --wsgi-file app.py --callable app --master"
