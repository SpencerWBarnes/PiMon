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

    while True:
        # Only poll if alive
        if(isPollAlive()):
            redisConnection.publish('toArduino', '%s:%s' % (replyChannel, message))
            reply = subscriptions.get_message(timeout=interval)
            # If the reply is good
            if reply != None and reply['type'] == 'message':
                redisConnection.set('msg', reply['data'])
                
        sleep(interval)
        # Always try to stop polling, consumer must actively call 
        #   keepPollAlive to continue polling
        global pollAlive 
        pollAlive = False


# To be used by app.py thread to keep polling Arduino
def keepPollAlive():
    global pollAlive 
    pollAlive = True

# To be used by requesting thread to keep polling
def isPollAlive():
    return global pollAlive

def setupArduinoPolling(interval):
    # Spin up separate thread
    Thread(target=serialReader, args=(interval,), daemon=True).start()
