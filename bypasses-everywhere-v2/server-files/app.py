from flask import Flask, request, render_template, redirect
from flask_csp.csp import csp_header
from random import randint
import os.path
import os
import json
import urllib.parse

import admin
import logging

urls_to_check = {}

app = Flask(__name__)
csp = {'default-src':"'none'",'script-src':"www.google.com", 'img-src': '*', 'report-uri': '', 'style-src': "'unsafe-inline'"}

@app.route('/')
@csp_header(csp)
def home():
    articles = [
        ("my phone", "/article?name=Phone&time=32&unit=sec&img=https://i.ytimg.com/vi/HDZrmzhgmBY/maxresdefault.jpg&description=WoW such a fancy new phone, you should really buy it!"),
        ("my bed", "/article?name=Bed&time=20&unit=min&img=http://homesfeed.com/wp-content/uploads/2015/12/White-Modern-King-Size-Bed-Frame-With-Fur-Bed-And-Cool-Rug.jpg&description=I know you're tired. If I were you I'd love to sleep in this bed"),
        ("my bike", "/article?name=Bike&time=14&unit=h&img=http://bike-advisor.com/wp-content/uploads/2013/01/kids_bikes_3_to_5_years.jpg&description=That's the current bike I'm using (yeah I'm 30 so what?) but for you I can sell it I guess.."),
        ("my table", "/article?name=Table&time=9&unit=min&img=https://www.thakehamfurniture.co.uk/productimages/small-rosewood-centre-table-18-L.jpg&description=I'm selling this table, can fit from 20 to 25 people depending on their size and on your acrobatic skills"),
    ]
    return render_template('home.html', articles=articles)

@app.route('/redirect')
def redir():
    url = request.args.get("url")
    logging.error("Redirecting to :" + str(url))
    return render_template('redirect.html', **locals())

@app.route('/article')
@csp_header(csp)
def xss():
    name, time, unit, img, description = [request.args.get(i, "#") for i in ["name", "time", "unit", "img", "description"]]
    price = randint(300, 4000)
    return render_template('article.html', **locals())

def _checkurl(url):
    if not url:
        return "Invalid URL"
    if len(url) > 350:
        app.logger.warning("Url too long: " + url)
        return "that's way too long"

@app.route('/url', methods=["POST"])
@csp_header(csp)
def add_url_to_check():
    url = request.form.get("url")
    invalid_url = _checkurl(url)
    if invalid_url:
        return invalid_url
    url = urllib.parse.quote(url)
    url = f"http://127.0.0.1:8080/redirect?url={url}"
    global urls_to_check
    ip = request.headers.get("X-Forwarded-For", "none")
    reset_file = "/tmp/urls_done"
    if os.path.isfile(reset_file):
        urls_to_check = {}
        os.unlink(reset_file)
    urls_to_check[ip] = url
    urls_to_check_list = list(urls_to_check.values())
    with open("/tmp/urls.json", "w") as f:
        json.dump(urls_to_check_list, f)
    app.logger.info("Added", url)
    return "Thanks, I'll check out this article in a bit :)"


admin.routes(app, csp)

if __name__ == "__main__":
    app.run("0.0.0.0")
