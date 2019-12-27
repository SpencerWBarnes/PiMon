#include <Arduino.h>
#include <NewPing.h>
#include "JsonSerialStream.h"

//DEBUG Library: For debug testing and performance testing
#include "DebugAndPerformanceTest.h"

// Constants
#define SONAR1_trig 12
#define SONAR1_echo 11
#define LIMITSWITCH1 8

// Globals
NewPing sonar(SONAR1_trig, SONAR1_echo);
String incoming;

void setup() 
{
  Serial.begin(115200);

  // Reserve a small size that I am confident we will not exceed
  incoming.reserve(50);
  incoming = "";

  pinMode(LIMITSWITCH1, INPUT_PULLUP);
}

void loop() 
{
  //DEBUG Call to any code needed for testing
  testCode();
}

// Get Sonar data in json style string
void getSonarData(NewPing sonarSensor, JsonSerialStream &outgoing)
{
  outgoing.addProperty("data", sonarSensor.ping_cm());
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
  // Add semi-random acknowledgement value that will be used as a unique 
  //  ID for each message
  outgoing.addProperty("ack",(int)millis());

  // This follows the JSON format
  // Note that only numeric data has units, other sensors can be simple objects
  // numeric data: "sensorName":{"data":<data>,"units":<units>}
  // Simple data: "sensorName":<data>
  // message data: {"ack":<millis>,"sensorName":{sensorData},"sensorName":<sensorData>}\n
  
  //Sonar sensors:
  // Since sonar data has multiple properties: data, units. It is a nested 
  //  json object with internal properies to it. 
  // It is important that the nested object is closed
  // sonar
  outgoing.addNestedObject("sonar1");
  getSonarData(sonar, outgoing);
  outgoing.closeNestedObject();

  //Limit switch sensors:
  // Since switchdata has only one property it does not need to be nested
  // LIMITSWITCH1
  outgoing.addProperty("limitSwitch1", getLimitSwitchData(LIMITSWITCH1));
}

// Serial input parser
void serialEvent()
{
  // Recieve data byte by byte
  while (Serial.available())
  {
    char readString = Serial.read();
    incoming += readString;
  }

  // Interpret command once command ends
  // Only one command allowed at a time
  if (incoming.charAt(incoming.length()-1) == '\n')
  {
    // Make commands uniform, ignore casing and leading/trailling whitespace
    incoming.trim();
    incoming.toLowerCase();

    // Once the stream is opened, it must be closed
    JsonSerialStream outgoing = JsonSerialStream();

    // Command interpreters
    cmdGetSensors(incoming, outgoing);

    //DEBUG Calls to debugging Functions 
    getTimingData(outgoing);
    //getMemoryData(outgoing);
    //getTestData(outgoing);

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