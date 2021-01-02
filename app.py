import os
import sys
import time

import redis
from flask import Flask, render_template, Response, send_from_directory
from flask_assets import Environment, Bundle

# import function to start reading/writing from serial port
import arduinoPoller

red = redis.StrictRedis()


def create_app():
    # use error as stdout
    sys.stdout = sys.stderr

    # create Flask application
    app = Flask(__name__)

    # render SCSS
    scss = Bundle('site.scss', filters='libsass', output='site.css')
    assets = Environment(app)
    assets.url = app.static_url_path
    assets.register('scss_all', scss)

    # Spin up polling thread
    arduinoPoller.setupArduinoPolling(0.333)

    # route to favicon
    @app.route('/favicon.ico')
    def favicon():
        return send_from_directory(os.path.join(app.root_path, 'static'), 'favicon.ico')

    @app.route('/')
    def home():
        """Page: home of site"""
        return render_template('index.html')

    @app.route("/stream/console")
    def console_stream():
        """Stream: continuous pipeline stream
        This stream is connected to by client via Javascript to constantly download messages (Responses) from the server.
        See ./templates/index.html for Javascript code.

        """

        def events():
            while True:
                yield red.get('msg')
                arduinoPoller.keepPollAlive()
                time.sleep(.1)

        return Response(events(), mimetype='text/plain')

    return app 


if __name__ == '__main__':
    from waitress import serve

    serve(create_app(), host='0.0.0.0', port='80')