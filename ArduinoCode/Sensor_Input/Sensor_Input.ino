#include <Arduino.h>
#include <NewPing.h>
#include "JsonStringBuilder.h"

//DEBUG Library: For debug testing and performance testing
#include "DebugAndPerformanceTest.h"

#define SONAR1_trig 12
#define SONAR1_echo 11
#define LIMITSWITCH1 8

NewPing sonar(SONAR1_trig, SONAR1_echo);
String incoming;

void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  incoming.reserve(50);
  incoming = "";

  pinMode(LIMITSWITCH1, INPUT_PULLUP);
}

void loop() 
{
  testCode();
}

// Get Sonar data in json style string
JsonStringBuilder getSonarData(NewPing sonarSensor)
{
  JsonStringBuilder output = JsonStringBuilder(2,6);
  output.add("data", sonarSensor.convert_cm(sonarSensor.ping_median(3)));
  output.add("units", String("cm"));
  return output;
}

// Get Limit Switch data in json style string
bool getLimitSwitchData(int switchPin)
{
  return digitalRead(switchPin);
}

// Aggregate data into message to be sent to Pi
void getSensorData(JsonStringBuilder &outgoing)
{
  outgoing.add("ack",millis()%2000);

  // This follows the JSON format
  // Note that only numeric data has units, other sensors can be simple objects
  // numeric data: "sensorName":{"data":<data>,"units":<units>}
  // Simple data: "sensorName":<data>
  // message data: {"ack":<millis>,"sensorName":{sensorData},"sensorName":<sensorData>}\n
  
  //Sonar sensors:
  // sonar
  outgoing.add("sonar1",getSonarData(sonar));

  //Limit switch sensors:
  // LIMITSWITCH1
  outgoing.add("limitSwitch1", getLimitSwitchData(LIMITSWITCH1));
}

// Serial input parser
void serialEvent()
{
  // recieve command, only one allowed
  while (Serial.available())
  {
    String readString = Serial.readString();
    incoming += readString;
  }
  incoming.trim();
  // Constructor values are the expected 90% range. I expect it to be <= 6 properties with an average 
  //  value of 10 bytes 90% of the time 
  JsonStringBuilder outgoing = JsonStringBuilder(6,10);

  // command interpreters
  cmdGetSensors(incoming, outgoing);

  //DEBUG Calls to debugging Functions 
  getPerformanceData(outgoing);
  getTestData(outgoing);

  // reply if desired
  if (!outgoing.empty())
  {
    Serial.println(outgoing.getJsonString());
  }
  incoming = "";
}

bool cmdGetSensors(String command, JsonStringBuilder &outgoing)
{
  if (command.compareTo("get sensors") == 0)
  {
    getSensorData(outgoing);
    return true;
  }
  return false;
}