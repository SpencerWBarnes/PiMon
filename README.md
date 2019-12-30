# PiMon
Designed for Mississippi State University IEEE SECON Robotics team in Software Engineer Senior Project 2019.
Provides a web interface to view robot diagnostics data.


## Authors
Spencer Barnes, Jake Griesmer, Jordan Stremming, Tyler Whiticker


## Installation
This project uses **Python 3.7** and the **pipenv** package.
To install Pipenv, run `pip3 install pipenv`.

Clone this repository into a folder of your choosing and run
`pipenv install`. This will install all necessary packages for this project.

*This app will need to run as **root** to get port 80 access!*

You may run into issues with `libsass` and `pipenv`. To fix this,
enter your development folder and run `sudo pipenv shell`. Then,
type `pipenv install libsass`. This bypasses `pipenv` but fixes the bug.


## Setup
The Pi must be configured as a wireless access point. This requires:
* Wireless abilities, built in or as a dongle
* Set up as a DNS server
* Set up as a DHCP server
* Set IP address of host device (Pi)

In order to run the service on bootup, a script must be added to /etc/init.d/
and another to be added to /etc/rc2.d/.


## Development
PiMon is developed with the following:
* Python 3.7 + pipenv
* Bootstrap 4.3.1 + Bootswatch (Darkly)
* Flask
* Waitress

You may need C++ Build Tools to install all packages.

## Usage
PiMon runs on a Waitress server.

Run the app my executing the following command in the dev folder:
```
sudo pipenv run python app.py
```
or
```
sudo pipenv shell
python app.py
```
