#include <Arduino.h>
#include <NewPing.h>
#include "JsonSerialStream.h"

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
void getSonarData(NewPing sonarSensor, JsonSerialStream &outgoing)
{
  outgoing.addProperty("data", sonarSensor.convert_cm(sonarSensor.ping_median(3)));
  outgoing.addProperty("units", "cm");
}

// Get Limit Switch data in json style string
bool getLimitSwitchData(int switchPin)
{
  return digitalRead(switchPin);
}

// Aggregate data into message to be sent to Pi
void getSensorData(JsonSerialStream &outgoing)
{
  outgoing.addProperty("ack",(int)millis());

  // This follows the JSON format
  // Note that only numeric data has units, other sensors can be simple objects
  // numeric data: "sensorName":{"data":<data>,"units":<units>}
  // Simple data: "sensorName":<data>
  // message data: {"ack":<millis>,"sensorName":{sensorData},"sensorName":<sensorData>}\n
  
  //Sonar sensors:
  // sonar
  outgoing.addNestedObject("sonar1");
  getSonarData(sonar, outgoing);
  outgoing.closeNestedObject();

  //Limit switch sensors:
  // LIMITSWITCH1
  outgoing.addProperty("limitSwitch1", getLimitSwitchData(LIMITSWITCH1));
}

// Serial input parser
void serialEvent()
{
  // recieve command, only one allowed
  while (Serial.available())
  {
    char readString = Serial.read();
    incoming += readString;
  }

  // Interpret command once command ends
  if (incoming.charAt(incoming.length()-1) == '\n')
  {
    incoming.trim();
    incoming.toLowerCase();

    // Once opened, it must be closed
    JsonSerialStream outgoing = JsonSerialStream();

    // command interpreters
    cmdGetSensors(incoming, outgoing);

    //DEBUG Calls to debugging Functions 
    getPerformanceData(outgoing);
    getTestData(outgoing);

    outgoing.closeMessage();
    incoming = "";
  }
}

bool cmdGetSensors(String command, JsonSerialStream &outgoing)
{
  if (command.compareTo("get sensors") == 0)
  {
    getSensorData(outgoing);
    return true;
  }
  return false;
}