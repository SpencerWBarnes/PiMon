// This library was written as a Software Engineering Senior Project for 
//  the CSE Department of Mississippi State University
// Authors: Spencer Barnes, Jordan Stremming, Tyler Whiticker, Jake Griesmer

// A collection of functions to assist in the sending data from an Arduino 
//  board and its sensors to an external machine. To be used in conjunction 
//  with JsonSerialStream
// This library is intended to be expanded upon to handle more sensors.

#include "SensorMonitor.h"

/*
  Ultrasonic or Sonar Sensors
*/
// Add sonar data object from New Ping ultrasonic sensors to Stream
// {"data":<int>, "units":"cm"}
void getSonarObject(NewPing sonarSensor, JsonSerialStream &outgoing)
{
  outgoing.addProperty("data", sonarSensor.ping_cm());
  outgoing.addProperty("units", "cm");
}


/*
  Limit switch or Pin input Sensors
*/
// Returns value of Arduino Pin wired to limit switch
bool getLimitSwitchData(int switchPin)
{
  return digitalRead(switchPin);
}