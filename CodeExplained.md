# PiMon's code explained

## Inital Setup
<details> <summary> Expand </summary> 

### Hardware
* Arduino
  * A low level microcontroller that works closely with hardware (Uno, Mega, Due). These devices are highly limited memory, single thread, and slow. Written in Arduino C.
* Raspberry Pi
  * A high level microcomputer. Has moderate memory, multiple threads, and fast. Written in Python.
* A client
  * This is any device connected to the website (phone, laptop, Pi). Uses a browser to access, view, and run the website. Written in JavaScript, CSS, HTML.

### File structure
Arduino
* Primary code is in: ArduinoCode/Sensor_Input/Sensor_Input.ino
* Libraries found in: ArduinoCode/Sensor_Input/Src/


Raspberry Pi
* Primary Code is in: app.py
* Launched in separate thread: arduinoPoller.py


Client
* Webpage HTML: templates/root.html **and** templates/index.html
* Webpage CSS: static/
* Webpage JavaScript: static/JavaScript/


### Code implementation
#### Arduino
Due to the Arduino having limited capability, its code must be written with the intent of being used with PiMon. Its code must follow a similar structure to that found in Sensor_Input.py. The Pi will send the command "get sensors\n" to the Arduino, and it must catch this command with serialEvent() _(this is a reserved function name for this purpose)_. Then the Arduino must interpret and respond with whatever data it wishes. This data will be used by PiMon.

#### Pi
Why an access point?
This allows the Pi to easily, freely, and securely serve the website. It also means it is always available and not dependant on the network in the area.

What is ReDis?
It offers a way to share data between processes by storing it in a dictionary (as seen between the arduinoPoller.py and app.py). It also offers a Publish/Subscribe service between processes (as seen with the serialManager process). **Warning**: this does require initial set up on the Pi.

What is Waitress and Flask?
Flask is what sets up the server's replies. It tells what commands to look for and what to respond with when the client requests.
Waitress is what sets up the server actually interprets the incoming messages. It is what makes the website accessible.

What the heck is going on in the HTML?
Flask offers a precompiler called Jinja2. This allows the server to manipulate the HTML before sending it to the client. 

</details>

## Runtime
<details> <summary> Expand </summary>

</details>