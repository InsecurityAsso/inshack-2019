from flask import Flask, render_template, request
app = Flask(__name__, static_url_path='/static')

milestones = [
    (150, 'The first flag is INSA{N0t_bad_f0r_a_start}. The next flag will be awarded at <= 135.'),
    (135, 'INSA{135_is_pretty_g0Od_but_how_l0w_c4n_u_gO}. Get your next flag at <= 128'),
    (128, 'INSA{Getting_cl0ser}. The last flag is waiting for you at 126 !'),
    (126, 'INSA{Master_of_0pt1mizatioN}. 126 is the best solution we could find, please contact @Mathis_Hammel or another admin if you find lower and we\'ll award you a few bonus points !')
    ]

ROUTES = []
with open('static/routes.txt', 'r') as fi:
    for line in fi.read().strip().split('\n'):
        ROUTES.append(line.split(','))

def check_solution(tap_list, route_list):
    taps = set(tap_list)
    for route in route_list:
        if all([elt not in taps for elt in route]):
            return 'Missing coverage for route %s' % ','.join(route)

    score = len(tap_list)
    result = ''
    for mscore, mmess in milestones:
        if score <= mscore:
            result += mmess + '<br>\n'
    return result or 'No flags for you, too many taps'

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/submit', methods=['GET', 'POST'])
def submit():
    tap_list = request.form['taps'].replace(',', ' ').strip().split()
    return check_solution(tap_list, ROUTES)

if __name__ == '__main__':
    app.run('0.0.0.0', 4321, threaded=True, debug=False)
