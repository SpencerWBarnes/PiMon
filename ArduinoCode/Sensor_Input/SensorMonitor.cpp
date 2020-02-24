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

/*
  MPU6050 Gyro and Acceleratometer Sensors
*/
// Add gyro angular velocity object from MPU6050 sensors to Stream
// {"scale":<uint8_t>, "x":<int16_t>, "y":<int16_t>, "z":<int16_t>}
void getGyroRotationObject(MPU6050 gyroSensor, JsonSerialStream &outgoing)
{
  int16_t x, y, z;
  gyroSensor.getRotation(&x, &y, &z);

  outgoing.addProperty("scale", gyroSensor.getFullScaleGyroRange());
  outgoing.addProperty("x", x);
  outgoing.addProperty("y", y);
  outgoing.addProperty("z", z);
}

// Add gyro axial acceleration object from MPU6050 sensors to Stream
// {"x":<int16_t>, "y":<int16_t>, "z":<int16_t>}
void getGyroAccelerationObject(MPU6050 accelerometer, JsonSerialStream &outgoing)
{
  int16_t x, y, z;
  gyroSensor.getAcceleration(&x, &y, &z);

  outgoing.addProperty("scale", gyroSensor.getFullScaleAccelRange());
  outgoing.addProperty("x", x);
  outgoing.addProperty("y", y);
  outgoing.addProperty("z", z);
}