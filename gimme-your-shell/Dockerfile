FROM ubuntu:bionic

RUN apt-get update && apt-get install openssh-server python3.7 docker.io cron pwgen --no-install-recommends -y

# user doit avoir des privileges root pour acceder au socket docker
RUN useradd -ou 0 -g 0 --home-dir /home/user user

ADD server-files/banner /etc/banner
ADD sshd_config /etc/ssh/sshd_config
ADD server-files/sandbox_start.sh /home/user/sandbox_start.sh

# On met en place le cron de kill de conteneur après 10 minutes
RUN echo '* * * * * root docker kill $(docker ps -f "name=gimme-your-shell-sandbox" -f "status=running" | grep -P "Up \d+ minutes" | cut -d" " -f 1)' >> /etc/crontab && \
    echo >> /etc/crontab
RUN chown user: /home/user/sandbox_start.sh && chmod u+x /home/user/sandbox_start.sh && chsh -s /home/user/sandbox_start.sh user

USER user

EXPOSE 22

CMD export HOME=/home/user && service docker start && docker login -u ${DOCKER_USER} -p ${DOCKER_PASSWORD} registry-chal.infra.insecurity-insa.fr && \
    service ssh restart && service cron start && \
    tail -f /var/log/docker.log
