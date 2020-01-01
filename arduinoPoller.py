from threading import Thread
from time import sleep
import redis

# Lazy start polling
pollAlive = False

def requestSensorData(interval):
    redisConnection = redis.StrictRedis(decode_responses=True)
    subscriptions = redisConnection.pubsub()

    # Current configuration
    replyChannel = "fromArduinoSensors"
    message = "get sensors"

    # Subscribe to reply channels
    subscriptions.subscribe(replyChannel)

    while True:
        # Only poll if alive
        if(isPollAlive()):
            redisConnection.publish('toArduino', '%s:%s' % (replyChannel, message))
            reply = subscriptions.get_message(timeout=interval)
            # If the reply is good
            if reply != None and reply['type'] == 'message':
                redisConnection.set('msg', reply['data'])
      
            # Always try to stop polling
            stopPolling()

        # Wait before sending new poll
        # Consumer must actively keepPollAlive during this time to keep polling
        sleep(interval)

# To be used by app.py thread to keep polling Arduino
def keepPollAlive():
    global pollAlive 
    pollAlive = True

# To be used by requesting thread to stop polling unless specifically told otherwise
def stopPolling():
    global pollAlive
    pollAlive = False

# To be used by requesting thread to keep polling
def isPollAlive():
    return pollAlive

def setupArduinoPolling(interval):
    # Spin up separate thread
    Thread(target=requestSensorData, args=(interval,), daemon=True).start()
