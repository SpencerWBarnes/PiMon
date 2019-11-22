import os
import time

from flask import Flask, render_template, Response, send_from_directory
from flask_assets import Environment, Bundle

# create Flask application
app = Flask(__name__)

# render SCSS
scss = Bundle('site.scss', filters='libsass', output='site.css')
assets = Environment(app)
assets.url = app.static_url_path
assets.register('scss_all', scss)

# open log file for display
LOG_FILE_NAME = "/home/pi/PiMon/_std.log"
log_file = None


# route to favicon
@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'favicon.ico')


@app.route('/')
def home():
    """Page: home of site"""
    return render_template('index.jinja2')


@app.route("/stream/console")
def console_stream():
    """Stream: continuous pipeline stream
    This stream is connected to by client via Javascript to constantly download messages (Responses) from the server.
    See ./templates/index.jinja2 for Javascript code.

    """
    # log_file is global
    global log_file

    # if the log_file has not been opened
    if log_file is None:
        try:
            # try to open LOG_FILE_NAME
            log_file = open(LOG_FILE_NAME, os.O_NONBLOCK)
        except FileNotFoundError:
            # something went wrong
            return Response("Nothing to show.")

    def stream():
        # go to last line of file
        log_file.seek(0, 2)
        # loop indefinitely
        while True:
            # read a line
            line = log_file.readline()
            # if line didn't exist
            if not line:
                # wait
                time.sleep(0.1)
                continue
            # yield the line
            yield line

    return Response(stream(), mimetype='text/html')


# if main process, run the Flask app
if __name__ == '__main__':
    # run app on 0.0.0.0 to accept all connections
    app.run(host='0.0.0.0')
