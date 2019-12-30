import redis
import serial
import serial.tools.list_ports
import time

'''
* Helpful links
* Very basic example of PubSub
    https://medium.com/@johngrant/python-redis-pub-sub-6e26b483b3f7
* How to programmatically find open ports
    https://stackoverflow.com/questions/24214643/python-to-automatically-select-serial-ports-for-arduino
* Explanation of how subscription messages behave
    https://stackoverflow.com/questions/27745842/redis-pubsub-and-message-queueing
* More redis details if you want/need to know
    https://making.pusher.com/redis-pubsub-under-the-hood/
* The redisPy GitHub, has documentation but is incomplete
    https://github.com/andymccurdy/redis-py#publish--subscribe
* The true redisPy code and documentation
    https://redis-py.readthedocs.io/en/latest/_modules/redis/client.html
'''


def main():
  # Define default configuration
  requestChannel = 'toArduino'
  managerChannel = 'serialManager'
  baudrate = 115200
  serialTimeout = 2

  # establish redis and pubsub connections 
  redisConnection = redis.StrictRedis()
  subscriptions = redisConnection.pubsub(ignore_subscribe_messages=True)
  subscriptions.subscribe(requestChannel)

  # establish serial communcation
  arduinoPort = getArduinoPort() #blocking
  arduinoSerial = serial.Serial(arduinoPort, baudrate=baudrate, timeout=serialTimeout)
  redisConnection.publish(managerChannel, f'{arduinoPort} connected')

  for request in subscriptions.listen(): #blocking
    # Extract where to send Arduino's reply and the message to send
    replyChannel, message = request['data'].split(":",1)

    # Handle Requests
    if request.channel == requestChannel:
      if isPortAlive():
        # Send message
        arduinoSerial.write(message.encode())
        # Wait for reposponse from Arduino
        response = arduinoSerial.readline().decode('utf-8')
        # Publish response to reply channel
        redisConnection.publish(replyChannel, response)
      
      # Arduino serial connection is dead
      else:
        # Close the now disconnected serial port
        arduinoSerial.close()
        # Inform that Arduino connection is dead
        redisConnection.publish(managerChannel, f'{arduinoPort} disconnected')

        # Clear message queue, no timeout is sent to the consumer processes
        #   it is important that consumers can recover from no response
        subscriptions.unsubscribe(arduinoSerial)

        # Reestablish connection
        arduinoPort = getArduinoPort() #blocking
        arduinoSerial = serial.Serial(arduinoPort, baudrate=baudrate, timeout=serialTimeout)
        redisConnection.publish(managerChannel, f'{arduinoPort} reconnected')
  
  # Infinite loop never reaches this return
  return 
 

'''
* Port Helper functions
'''
# Identify the port that an Arduino would be on 'ttyACM#'
def findArduinoPort():
  arduinoPorts = list();
  # Grab all live serial comms
  for port in serial.tools.list_ports.comports():
    # Arduinos operate on ttyACM, a somewhat rare protocol, unlikely collision
    if 'ttyACM' in port.device:
      arduinoPorts.append(port.device)

  # None found
  if len(arduinoPorts) == 0:
    return None
  
  # Give first port
  return arduinoPorts[0]

# Check that a given port is still listed as active
def isPortAlive(port):
  openPorts = list(serial.tools.list_ports.comports())
  return port in openPorts

# Blocking function until an Arduino port is found
def getArduinoPort():
  while findArduinoPort() == None:
    time.sleep(1)
  return findArduinoPort()
  
