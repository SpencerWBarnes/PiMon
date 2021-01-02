import os
import sys
import time

import redis
import logging
from logging.handlers import RotatingFileHandler
from flask import Flask, render_template, Response, send_from_directory
from flask_assets import Environment, Bundle

import json

# import function to start reading/writing from serial port
import arduinoPoller

red = redis.StrictRedis()

logger = logging.getLogger('RedisDataStreams')
dirName = '/tmp/logFiles'

if not(os.path.exists(dirName)):
    os.makedirs(dirName)

handler = RotatingFileHandler(filename=os.path.join(dirName, 'RedisDataStreams.log'), 
                            maxBytes=(1024*1024),
                            backupCount=1)
formatter = logging.Formatter("%(asctime)s %(name)-12s %(levelname)-8s %(message)s", datefmt="%m/%d/%Y %I:%M:%S")
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(0)


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
                logger.info('Getting redis data streams')
                arduinoData = str(red.get('msg'))
                logger.debug('Arduino data: ' + arduinoData)
                if (arduinoData != None):
                    arduinoData = json.loads(arduinoData)
                    monitorData = get_pi_logs(arduinoData)
                    logger.debug('Monitor Data: ' + str(monitorData))
                    yield json.dumps(monitorData)

                arduinoPoller.keepPollAlive()
                time.sleep(.1)

        return Response(events(), mimetype='text/plain')

    return app 


def get_pi_logs(dataDictionary):
    logger.info('Getting pi logs')
    if (red.exists('piLogStreams')):
      streamNames = json.loads(str(red.get('piLogStreams')))
      for name in streamNames:
          if (red.exists(name)):
              dataDictionary[name] = red.get(name)
    logger.debug('Data Dictionary: ' + str(dataDictionary))
    return dataDictionary

if __name__ == '__main__':
    from waitress import serve

    logger.info('Starting server')
    serve(create_app(), host='0.0.0.0', port='80')