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
String logData;

void setup() 
{
  Serial.begin(115200);

  // Please be aware that the Uno ONLY has 2KB of RAM
  // Reserve a small size that I am confident we will not exceed
  incoming.reserve(50);
  incoming = "";

  // Reserve large but not lethal size for logs
  logData.reserve(500);
  logData = "";

  pinMode(LIMITSWITCH1, INPUT_PULLUP);
}

void loop() 
{
  //DEBUG Call to any code needed for testing
  testCode();
  //TEST
  if (dummyData >= 60000)
  {
    addLog(String("Test.ticker"), String("."));
  }
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

bool addLog(String name, String data)
{
  // Dangerously long
  if (logData.length() + name.length() + data.length() >= 495)
    return false;

  logData += name;
  logData += '\v'; // This is a vertical tab, a completely unused char
  logData += data;
  logData += '\v';
  return true;
}

void getLogs(JsonSerialStream &outgoing)
{
  unsigned int logLen = logData.length();
  unsigned int i = 0;

  String name;
  name.reserve(25);
  name = "";
  String data;
  data.reserve(100);
  data = "";

  while (i < logLen)
  {
    // getLogSegment handles the i increments
    i = getLogSegment(name, i, logLen);
    i = getLogSegment(data, i, logLen);
    outgoing.addProperty(name, data);
    name = "";
    data = "";
  }
  // Clear log data
  logData = "";
}

unsigned int getLogSegment(String &segment, unsigned int i, unsigned int stop)
{
  char currentChar;
  while (i < stop)
  {
    currentChar = logData.charAt(i);
    if (currentChar == '\v')
      return ++i;
    segment += currentChar;
    i++;
  }
  return i;
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

  //Log data
  // Since we do not know what logs to add, it will handle adding them
  getLogs(outgoing);
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