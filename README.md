# PiMon
Designed for the Mississippi State University IEEE SECON Robotics team in Software Engineer Senior Project 2019-2020
Provides a web interface to view robot diagnostics data.


## Authors
Spencer Barnes, Jake Griesmer, Jordan Stremming, Tyler Whiticker, Dylan Santa Cruz, Will Storey


## Installation

### Dependancies
On the Pi:
* Python3.7
* Pipenv for python3
* Redis server
* SerialManager process _[GitHub here](https://github.com/MSUSeconRobotics/SerialManager.git)_

On the Arduino:
* NewPing.h _Available in Arduino Library Manager_
* JsonSerialStream.h _[GitHub here](https://github.com/MSUSeconRobotics/JsonSerialStream)_

### Installation Instruction
To install Pipenv, run `pip3 install pipenv`.

Clone this repository and its submodules into a folder of your choosing with `git clone --recurse-submodules https://github.com/SpencerWBarnes/PiMon.git`. Then navigate into the repository and run `sudo pipenv install`. This will install all necessary packages for the Pi.

*This app will need to run as **root** to get port 80 access!*

You may run into issues with `libsass` and `pipenv`. To fix this,
enter your development folder and run `sudo pipenv shell`. Then,
type `pipenv install libsass`. This bypasses `pipenv` but fixes the bug.


## Setup
The Pi must already be configured:
1) As a wireless access point. Requiring:
   * Wireless abilities, built in or as a dongle
   * Set up as a DNS server *
   * Set up as a DHCP server **
   * Set IP address of host device (Pi)
2) With Redis server
3) With SerialManager _[GitHub here](https://github.com/MSUSeconRobotics/SerialManager.git)_

<sub>
* So the Pi can tell connected devices the IP address of websites
<br>
** So the Pi can give connected devices an IP address
</sub>
<br><br>

In order to run services on bootup, a script must be added to `/etc/init.d/`
and the run `sudo update-rc.d <FileName> defaults`.
> RC scripts explained [here](https://docs.oracle.com/cd/E19683-01/806-4073/6jd67r96g/index.html)


## Development
PiMon is developed with the following:
* Python 3.7 + pipenv
* Bootstrap 4.3.1 + Bootswatch (Darkly)
* Flask
* Waitress

You may need C++ Build Tools to install all packages.

If you want an explanation or description of the code, reference the [Code Explained doc](/CodeExplained.md).

## Usage
PiMon runs on a Python Waitress server. Connect to its WiFi network `SECONboi` with password `secon1337`, then go to the webpage's address at `192.168.1.1`

Run the app anywhere with:
```
sudo /etc/init.d/pimon start
```
and to stop the app:
```
sudo /etc/init.d/pimon stop
```

The app can also be run by executing the following command in the dev folder:
```
sudo pipenv run python app.py
```
or
```
sudo pipenv shell
python app.py
```

## Logging Explained
All logs will be output to the PiMon webpage as well as to the log files found at `/tmp/logFiles/`. This data is pushed each time a client requests an update through the `stream/console` endpoint. the log files on the Pi may include other logging data as well if desired.  

#### Arduino:
To log sensors or logging streams on the Arduino, use the SensorMonitor and Logger C++ libraries to create the logs. Then use the JsonSerialStream C++ library to output those logs across the USB connection to the Pi. Arduinos have severely limited memory capabilities, so their logging abilities are short term and are dependent on their own workload and how frequently the Pi requests for the data off the Arduino.

#### Pi:
To log sensors or logging streams on the Pi, use redis to set a value that can then be accessed across processes, ex: `redisConnection.set('Pi<StreamName>', '<StreamData>')`. These Pi logging streams must use the prefix `Pi` at the beginning of their stream name to be included, as seen in the example before.

## Resources
* Explanation on how to upload to a arduino board via commandline on a pi [here](https://www.woolseyworkshop.com/2019/04/14/using-the-arduino-command-line/)
* Explanation on what Jinja2 is and how to use it [here](https://codeburst.io/jinja-2-explained-in-5-minutes-88548486834e)
* Expalnation of init.d scripts and how they are used on startup [here](https://www.poftut.com/what-is-init-d-and-how-to-use-for-service-management-in-linux/)

---
Please note that due to unanticipated circumstances _(global pandemic)_ we were not able to set up a second Pi with this configuration. 
We sincerely apologize if the above steps are incomplete, and **we would appreciate it if users would add to this repository**. 
