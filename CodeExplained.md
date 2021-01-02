# PiMon's code explained

This document is to give some explanation of how the code was designed, what is necessary, what the different parts do, and answer some questions of why choices were made.

It may be incomplete and you may find places you could improve, to which you are encouraged to reach out to ask questions to add to this document.

If you want a user's guide rather than development guide, go to the [ReadMe](/README.md).

## Inital Setup

#### Hardware

<details> <summary> Expand </summary> 

* **Arduino** - a low level microcontroller that works closely with hardware (Uno, Mega, Due). These devices are highly limited memory, single thread, and slow. Written in Arduino C.
* **Raspberry Pi** - a high level microcomputer. Has moderate memory, multiple threads, and fast. Written in Python.
* **A client** - this is any device connected to the website (phone, laptop, Pi). Uses a browser to access, view, and run the website. Written in JavaScript, CSS, HTML.

</details><br>

#### File structure

<details> <summary> Expand </summary> 

Arduino
* Primary code is in: ArduinoCode/Sensor_Input/Sensor_Input.ino
* Libraries found in: ArduinoCode/Sensor_Input/Src/


Raspberry Pi
* Primary Code is in: app.py
* Launched in separate thread: arduinoPoller.py


Client
* Webpage HTML: templates/root.html **and** templates/index.html
* Webpage JavaScript: static/JavaScript/
* Webpage CSS: static/

</details><br>

#### Code implementation

<details> <summary> Expand </summary> 

#### Arduino
Due to the Arduino having limited capability, its code must be written with the intent of being used with PiMon. Its code must follow a similar structure to that found in Sensor_Input.py. The Pi will send the command "get sensors\n" to the Arduino, and it must catch this command with serialEvent() _(this is a reserved function name for this purpose)_. Then the Arduino must interpret and respond with whatever JSON formatted data it wishes. This data will be used by PiMon.

#### Pi
1) Why an access point?
This allows the Pi to easily, freely, and securely serve the website. It also means it is always available and not dependant on the network in the area.

2) What is ReDis?
It offers a way to share data between processes by storing it in a dictionary (as seen between the arduinoPoller.py and app.py). It also offers a Publish/Subscribe service between processes (as seen with the serialManager process). **Warning**: this does require initial set up on the Pi.

3) What is Waitress and Flask?
Flask is what sets up the server's replies. It tells what commands to look for and what to respond with when the client requests.
Waitress is what sets up the server and actually interprets the incoming messages. It is what makes the website accessible.

4) What the heck is going on in the HTML?
Flask offers a precompiler language called Jinja2. This allows the server to manipulate the HTML before sending it to the client. The file root.html gives the basic form of the webpage as well as Jinja2 "blocks". This allows index.html to only define what goes in these "blocks". _It is a little complicated, but it is neat._ 

</details><br>

## Runtime

#### Before execution
<details> <summary> Expand </summary>

For obvious reasons, the Arduino must be plugged into the Pi by USB. It must be capable of interpretting and responding to the `get sensors` command.

The Pi must be an available access point, running a ReDis server, and running the SerialManager process. The access point should be automatic. Running the ReDis server and SerialManager processes can be done manually, or at bootup by altering rc2.d

</details>

#### Execution time
<details><summary> Expand </summary>

To launch PiMon open a terminal, navigate to the PiMon folder, and execute `sudo pipenv run python app.py`. This will launch the arduinoPoller.py in a separate thread, and it will make the webpage available at 192.168.1.1

</details>

## Description

### From a client's view 
The client will connect to the Pi's network. The Pi will then give the client an IP address so that it can send and recieve data.

When the client requests the PiMon webpage, the Pi will defer to the Waitress server. Waitress will interpret the Flask code and send the necessary data, in this case it will send index.html. Before sending, Jinja2 will complete its preprocessing where it substitutes the code blocks.

The client will now execute the webpage's embedded JavaScript code which sends a data request for /console_stream to the Pi and sets a function call every XX milliseconds.

Now every XX milliseconds the client will interpret the data from the Pi and update the webpage accordingly. The data is added dynamically, so the webpage is independent of what data or hardware the Pi and Arduino have. None of the data is stored long term with numeric and boolean data being deleted as soon as a new message is recieved and streams only storing the most recent ZZ number of characters.
<br>

### From the server's view
When the client requests new data by requesting /console_stream, it will return whatever data is held in ReDis, ask the arduinoPoller thread to keep polling, and sleep the thread so it is not constantly running.

The arduinoPoller thread is in parallel to the server. As long as it is asked to poll the Arduino, it will send a message every YY seconds to the SerialManager and listen for a response. The SerialManager will listen for a request, send the message to the Arduino, then publish the Arduino's reply to the channel provided to it.

The arduinoPoller listens to this reply channel. Once a response is recieved it updates the data stored in ReDis. The poller then shuts itself off, and another process (namely app.py) must manually call keepPollAlive. This is so that unless there is an active user, the poller does not message the Arduino.

The arduinoPoller then sleeps its thread so it is not constantly running.
<br>

### From the Arduino's view
The Arduino is single threaded. Each Arduino program follows a default structure. First any global commands are run, setup() is run once, then in an infinite while it runs loop() then serialEvent() if incoming serial data is detected. **This means that is loop does not release control quickly** (below 50ms) **the serialEvent handler may not work properly.**

The Arduino must aggregate incoming messages in the serialEvent() function. It will come in several characters at a time, so a buffer of characters must be filled until a line terminater is encountered.

For PiMon to work, the Arduino must have a function that interprets the serial message and acts accordinly if it is `get sensors`. The reply must be a properly formed JSON formatted string with at least an `ack` property and ending with a line terminater. The **JsonSerialStream** library was made to help with this.

Another useful module written was **ArduinoMonitor** which contains libraries Logger and SensorMonitor. Logger provides a way for Arduinos to store logging data, which can be coupled with PiMon to feed out logging data. SensorMonitor is a library with some common sensor types which return sensor data that can help assist in sending JSON data to PiMon.

Just as it is important that loop() is short so serialEvent() may run, it is important that sending serial data is fast so it does not impede the Arduino's operation.