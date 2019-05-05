FROM python:latest

RUN pip install flask gunicorn pytesseract flask_recaptcha deda pyotp && \
    apt-get update && apt-get upgrade -y &&\
    apt-get install -y tesseract-ocr &&\
    useradd python-runner

RUN mkdir -p /var/www
COPY server-files/ /var/www/

WORKDIR /var/www/
RUN chown -R python-runner:python-runner .
USER python-runner
EXPOSE 5000

CMD gunicorn server:app -b 0.0.0.0:5000 --access-logfile - --error-logfile -
