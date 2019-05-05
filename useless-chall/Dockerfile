FROM ubuntu:bionic

RUN apt-get update && apt-get install openssh-server python3.7 docker.io cron pwgen --no-install-recommends -y

RUN useradd --home-dir /home/user user && echo "user:user" | chpasswd user

ADD sshd_config /etc/ssh/sshd_config
ADD server-files/sandbox_start.sh /home/user/sandbox_start.sh
ADD server-files/hash /home/user/hash
ADD server-files/flag.txt /home/user/flag.txt

RUN chown user: /home/user/sandbox_start.sh && chmod u+x /home/user/sandbox_start.sh && chmod +x /home/user/hash && chsh -s /home/user/sandbox_start.sh user

EXPOSE 22

CMD service ssh restart && tail -f /var/log/lastlog

