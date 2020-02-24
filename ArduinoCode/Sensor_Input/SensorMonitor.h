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
#include "src/SubModules/MPU6050/MPU6050.h"

// Add sonar data object from New Ping ultrasonic sensors to Stream
// {"data":<int>, "units":"cm"}
void getSonarObject(NewPing sonarSensor, JsonSerialStream &outgoing);

// Returns value of Arduino Pin wired to limit switch
bool getLimitSwitchData(int switchPin);

// Add gyro angular velocity object from MPU6050 sensors to Stream
// {"scale":<uint8_t>, "x":<int16_t>, "y":<int16_t>, "z":<int16_t>}
void getGyroRotationObject(MPU6050 gyroSensor, JsonSerialStream &outgoing);

// Add gyro axial acceleration object from MPU6050 sensors to Stream
// {"scale":<uint8_t>, "x":<int16_t>, "y":<int16_t>, "z":<int16_t>}
void getGyroAccelerationObject(MPU6050 accelerometer, JsonSerialStream &outgoing);