from flask import request, render_template
from flask_csp.csp import csp_header
import requests
import re

with open("flag.txt") as f:
    FLAG = f.read()

def _local_access() -> bool:
    if not request.referrer:
        return False
    if not re.match(r"^http://127\.0\.0\.1(:\d+)?/", request.referrer):
        return False
    return request.remote_addr == "127.0.0.1"

def routes(app, csp):
    @csp_header(csp)
    @app.route("/admin", methods=["GET", "POST"])
    def adm():
        if request.method == "GET":
            url = request.args.get("picture")
            if _local_access():
                with open(__file__) as f:
                    code = f.read()
            else:
                code = None
            return render_template("admin.html", url=url, code=code)
        else:
            try:
                assert _local_access()
                data = request.get_json(force=True)
                assert data["secret"] == "No one will never ever access this beauty"
                requests.post(data["url"], data={
                    "flg": FLAG,
                }, timeout=2)
                return "yeah!"
            except Exception as e:
                app.logger.error(e)
                return 'Wut?'
