// This library was written as a Software Engineering Senior Project for 
//  the CSE Department of Mississippi State University
// Authors: Spencer Barnes, Jordan Stremming, Tyler Whiticker, Jake Griesmer

// Provide functions to assist in the recieving of data from an Arduino 
//  board and its sensors. To be used in conjunction with JsonSerialStream
// This library is intended to be expanded upon to handle more sensors.

#pragma once

#include <Arduino.h>
#include <NewPing.h>
#include "JsonSerialStream.h"

// Add sonar data object from New Ping ultrasonic sensors to Stream
// {"data":<int>, "units":"cm"}
void getSonarObject(NewPing sonarSensor, JsonSerialStream &outgoing);

// Returns value of Arduino Pin wired to limit switch
bool getLimitSwitchData(int switchPin);