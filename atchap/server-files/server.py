import smtplib
from email.utils import parseaddr

from flask import Flask, abort, render_template, request, send_from_directory
from flask_recaptcha import ReCaptcha
YAHOO=False
if YAHOO:
    SMTP_ADDRESS = "smtp.mail.yahoo.com"
else :
    SMTP_ADDRESS = "smtp.gmail.com"

app = Flask(__name__)
app.config.update(
    RECAPTCHA_SITE_KEY="6Lel958UAAAAAH8bYnyAUwfs7qywUi4oqJEf69R9",
    RECAPTCHA_SECRET_KEY=open(
        "private/config-6d37d626-6685-11e9-a923-1681be663d3e").readline()
)
if YAHOO:
    sender_mail, sender_pass = open(
            "private/config-7931c09f-8e5e-4066-a45b-ea614561dbb9.yahoo").readline().split(";")
else:
    sender_mail, sender_pass = open(
    "private/config-7931c09f-8e5e-4066-a45b-ea614561dbb9").readline().split(";")

recaptcha = ReCaptcha(app=app)


@app.route('/', methods=['GET'])
def hello_world():
    return render_template("index.html")


@app.route('/register', methods=['POST'])
def handle_register():
    if recaptcha.verify():
        input_mail = request.form.get('email')
        if input_mail:
            input_mail = input_mail.lower()
            whitelist_add = ["maud.erateur@almosttchap.fr",
                             "guy.liguili@almosttchap.fr", "samira.bien@almosttchap.fr"]

            try:
                if any([n in input_mail for n in whitelist_add]):
                    parsed_add = parseaddr(input_mail)[1]
                    if not "almosttchap.fr" in parsed_add:
                        # We don't want to send a message if ths input does not exploit the "bug"
                        flag = open("private/flag.txt").read()

                        s = smtplib.SMTP(SMTP_ADDRESS, 587)
                        s.starttls()
                        s.login(sender_mail, sender_pass)
                        message = "Subject: {}\n\n Here is your flag : {}. Remember to make responsible disclosure ;)".format(
                            "Welcome to societe", flag)
                        s.sendmail(sender_mail, parsed_add, message)
                        s.quit()
                        return render_template('result.html', result="Mail sent, check your spam folder and wait up to 5 minutes")
                    else :
                        return render_template('result.html', result="You're not using your official address..")
                else:
                    return render_template('result.html', result="You're not whitelisted or not part of the company..")
            except (KeyboardInterrupt, SystemExit):
                raise
            except Exception as e:
                print(e)
                s.quit()
                return render_template('result.html', result="An error occured")
        else:
            return render_template('result.html', result="Provide a mail")
    else:
        return render_template('result.html', result="Hello robot")


@app.route('/css/<path:path>')
def send_css(path):
    return send_from_directory('css', path)


@app.route('/img/<path:path>')
def send_img(path):
    return send_from_directory('img', path)


@app.route('/private/flag.txt')
def censorship():
    abort(403)


if __name__ == '__main__':
    app.run(host="127.0.0.1")
