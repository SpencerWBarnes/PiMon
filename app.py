import os
import sys
import time

import redis
from flask import Flask, render_template, Response, send_from_directory
from flask_assets import Environment, Bundle

# import function to start reading/writing from serial port
from serialHandler import setupSerialHandlers

red = redis.StrictRedis()


def create_app():
    # use error as stdout
    sys.stdout = sys.stderr

    # create Flask application
    app = Flask(__name__)

    # render SCSS
    scss = Bundle('site.scss', 'heartbeat.css', filters='libsass', output='site.css')
    assets = Environment(app)
    assets.url = app.static_url_path
    assets.register('scss_all', scss)

    # initialize serialHandlers
    setupSerialHandlers()

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

        def events():
            while True:
                yield "time,"+red.get('msg').decode('utf-8')
                time.sleep(.25)

        return Response(events(), mimetype='text/plain')

    return app


if __name__ == '__main__':
    from waitress import serve

    serve(create_app(), host='0.0.0.0', port='80')
