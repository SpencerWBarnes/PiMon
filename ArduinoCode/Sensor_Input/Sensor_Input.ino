#include <Arduino.h>
#include <NewPing.h>
//DEBUG Library: Used for measuring Heap Fragmentation
#include "MemoryInfo.h"

#define SONAR1_trig 12
#define SONAR1_echo 11
#define LIMITSWITCH1 8

NewPing sonar(SONAR1_trig, SONAR1_echo);

//DEBUG Variables: Dummy variables for timer performance and testing logs
unsigned dummyData = 0;
unsigned long avgDummyTime = 0;
unsigned long dummyStart = 0;

String incoming;

// Class to simplify creating JSON strings 
class JsonStringBuilder
{
  private:
    String message;

  public:
    // I wanted default parameters of 1 then 10, but Arduino can ONLY have defaults in the
    //  prototype, not in the defintion. When this is moved to a library, define defaults
    //  in the header file's prototypes
    JsonStringBuilder(int estimatedNumberOfElements, int estimatedElementSize)
    {
      message = "";
      message.reserve((estimatedElementSize+4)*estimatedNumberOfElements);
    }

    // Overload for string data, needs quotes \"
    void add(String propertyName, String propertyValue)
    {
      message += "\""+ propertyName +"\":\""+ propertyValue +"\",";
    }

    // Overload for other JSON builders, uses getJsonString and no quotes
    void add(String propertyName, JsonStringBuilder propertyValue)
    {
      message += "\""+ propertyName +"\":"+ propertyValue.getJsonString() +",";
    }

    // Overload for bool data, write as true/false not 1/0
    void add(String propertyName, bool propertyValue)
    {
      message += "\""+ propertyName +"\":"+ String(propertyValue?"true":"false") +",";
    }

    template <class T>
    void add(String propertyName, T propertyValue)
    {
      // Name requires quotes, thus they must be escaped as \"
      message += "\""+ propertyName +"\":"+ propertyValue +",";
    }

    String getJsonString()
    {
      // Cut off last comma , and encompass in braces {}
      return "{"+ message.substring(0, message.length()-1) +"}";
    }

    bool empty()
    {
      return message == "";
    }
};

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
  if (dummyData >= 60000)
  {
    avgDummyTime = ((avgDummyTime*3) + (millis() - dummyStart))/4;
    dummyData = 0;
    dummyStart = millis();
  }
  dummyData++;
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

//DEBUG Function: Get various performance statistics
void getPerformanceData(JsonStringBuilder &outgoing)
{
  // Show avg time to count dummyData
  outgoing.add("avgDummyTime", "{\"data\":"+String(avgDummyTime)+",\"units\":\"ms\"}");
  // Show heap fragmentation
  outgoing.add("Heap Fragmentation", "{\"data\":"+String(getFragmentation())+",\"units\":\"%\"}");
}

//DEBUG Function: Get data used just for testing puroses
void getTestData(JsonStringBuilder &outgoing)
{
  outgoing.add("tick", String("."));
  if (dummyData % 50 == 0) {
    outgoing.add("Dumb Chance", String(dummyData));
  }
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