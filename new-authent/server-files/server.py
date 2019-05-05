#!/usr/bin/python3
from flask import Flask,request,send_from_directory,render_template,abort
import pytesseract
from PIL import Image
from re import sub
from io import BytesIO
from flask_recaptcha import ReCaptcha
import time
import tempfile 
from libdeda.print_parser import comparePrints
import pyotp

app = Flask(__name__)
app.config.update(
    RECAPTCHA_SITE_KEY="6LeCM6AUAAAAAKMdJaM0enZ-bj3KdBQePY1u2gdp",
    RECAPTCHA_SECRET_KEY=open(
        "private/config-659114ac-0f23-4ea4-aeca-5cb4a4c798a3").readline(),
    MAX_CONTENT_LENGTH = 500 * 1024
)
recaptcha = ReCaptcha(app=app)
flag = open("private/flag.txt").read()
totp = pyotp.TOTP('HAZTIMRXGMZDI===')

@app.route('/',methods=['GET'])
def ind():
    return render_template("index.html")

@app.route('/login',methods=['POST'])
def login():
    if recaptcha.verify():
        if 'file' not in request.files:
            return render_template('result.html', result = "No file uploaded")
        file = request.files['file']
        if file and file.filename == '':
            return render_template('result.html', result = "No correct file uploaded")
        if file:
            try:
                image_content = Image.open(BytesIO(file.read()))
                image_content = remove_transparency(image_content)
                b_w_image = image_content.convert('L')
                input_text = pytesseract.image_to_string(
                    b_w_image , config='-c tessedit_char_whitelist=0123456789')
                print(input_text)
                input_text = input_text.replace('/','7')
                if any(i not in '0123456789' for i in input_text):
                    return render_template('result.html', result = "Your code should contains only numbers")
                if not totp.verify(input_text):
                    return render_template('result.html', result="Bad TOTP Code")
                tempf = tempfile.NamedTemporaryFile(suffix="_newauthent.png")
                image_content.save(tempf.name)
                if comparePrints(["private/ref.png", tempf.name])[2]:
                    return render_template('result.html', result = flag)
                return render_template('result.html', result="You're not using my printer !")
            except (KeyboardInterrupt, SystemExit):
                raise
            except IOError as e:
                print(e)
                return render_template('result.html', result="You have to upload a picture, not a PDF !")
            except  Exception as e :
                print(e)
                return render_template('result.html', result = "Something went wrong...")
        else:
            return render_template('result.html', result="Something went wrong...")
    else:
        return render_template('result.html', result = "Please be a human")

    return render_template('result.html', result="Something went wrong...")

@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)

@app.route('/css/<path:path>')
def send_css(path):
    return send_from_directory('css', path)

@app.route('/img/<path:path>')
def send_img(path):
    return send_from_directory('img', path)

@app.route('/debug')
def send_now():
    return str(int(time.time()))

@app.route('/private/flag.txt')
def censorship():
    abort(403)


def remove_transparency(im, bg_colour=(255, 255, 255)):

    # Only process if image has transparency (http://stackoverflow.com/a/1963146)
    if im.mode in ('RGBA', 'LA') or (im.mode == 'P' and 'transparency' in im.info):

        # Need to convert to RGBA if LA format due to a bug in PIL (http://stackoverflow.com/a/1963146)
        alpha = im.convert('RGBA').split()[-1]

        # Create a new background image of our matt color.
        # Must be RGBA because paste requires both images have the same format
        # (http://stackoverflow.com/a/8720632  and  http://stackoverflow.com/a/9459208)
        bg = Image.new("RGBA", im.size, bg_colour + (255,))
        bg.paste(im, mask=alpha)
        return bg

    else:
        return im

if __name__ == '__main__':
    app.run(host="0.0.0.0")
