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
* PySerial documentation
   https://buildmedia.readthedocs.org/media/pdf/pyserial/latest/pyserial.pdf 
'''


def main():
  # Define default configuration
  requestChannel = 'toArduino'
  managerChannel = 'serialManager'
  baudrate = 115200
  serialTimeout = 2

  # establish redis and pubsub connections 
  redisConnection = redis.StrictRedis(decode_responses=True)
  subscriptions = redisConnection.pubsub(ignore_subscribe_messages=True)
  subscriptions.subscribe(requestChannel)
  subscriptions.subscribe(managerChannel)

  # establish serial communcation
  arduinoPort = getArduinoPort() #blocking
  arduinoSerial = serial.Serial(arduinoPort, baudrate=baudrate, timeout=serialTimeout)
  redisConnection.publish(managerChannel, 'connected:%s' % arduinoPort)

  for request in subscriptions.listen(): #blocking
    # Handle Requests
    if request['channel'] == requestChannel:
      # Extract where to send Arduino's reply and the message to send
      # <channel to send reply>:<message to send to Arduino>
      try:
        replyChannel, message = request['data'].split(':',1)
      except:
        # Ignore malformed input
        print('Malformed input: %s' % request)
        continue

      if isPortAlive(arduinoPort):
        # Add line terminator
        message += '\n'
        # Send message
        arduinoSerial.write(message.encode())
        # Wait for reposponse from Arduino
        response = arduinoSerial.readline().decode('utf-8')
        # Publish response to reply channel
        redisConnection.publish(replyChannel, response)
      
      # Arduino serial connection is dead
      else:
        print('Serial port %s dead' % arduinoPort)
        # Close the now disconnected serial port
        arduinoSerial.close()
        # Inform that Arduino connection is dead
        redisConnection.publish(managerChannel, 'disconnected:%s' % arduinoPort)

        # Stop taking requests, no message is sent to the consumer processes
        #   it is important that consumers can recover from no response
        subscriptions.unsubscribe(requestChannel)

        # Reestablish connection
        arduinoPort = getArduinoPort() #blocking
        arduinoSerial = serial.Serial(arduinoPort, baudrate=baudrate, timeout=serialTimeout)
        redisConnection.publish(managerChannel, 'reconnected:%s' % arduinoPort)
        # Reopen to requests
        subscriptions.subscribe(requestChannel)
  
    # Handle management messages
    if request['channel'] == managerChannel:
      try:
        command, parameters = request['data'].split(':', 1)
      except:
        command = ''
        parameters = request['data']

      # broadcast:<message to echo>
      if command == 'broadcast':
        # Warning that this message will show in serialManager's messages
        redisConnection.publish(managerChannel, parameters)

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
  for livePort in serial.tools.list_ports.comports():
    if port == livePort.device:
      return True
  return False

# Blocking function until an Arduino port is found
def getArduinoPort():
  while findArduinoPort() == None:
    time.sleep(1)
  return findArduinoPort()
  
# Call main
if __name__ == '__main__':
    main()