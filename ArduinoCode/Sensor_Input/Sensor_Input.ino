#include <Arduino.h>
#include <NewPing.h>

#define SONAR1_trig 12
#define SONAR1_echo 11
#define LIMITSWITCH1 8

NewPing sonar(SONAR1_trig, SONAR1_echo);

//DEBUG PLEASE REMOVE Dummy variables for debugging
unsigned dummyData = 0;
bool dummyDataDirection = true;

String incoming = "";
String outgoing = "";

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

    void add(String propertyName, String propertyValue)
    {
      // Name and value requires quotes, thus they must be escaped as \"
      message += "\""+ propertyName +"\":\""+ propertyValue +"\",";
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
};

void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(2);

  pinMode(LIMITSWITCH1, INPUT_PULLUP);
}

void loop() 
{
  if (dummyData <= 0)
  {
    // Start going up
    dummyDataDirection = true;
  }
  else if (dummyData >= 60000)
  {
    // Start going down
    dummyDataDirection = false;
  }

  if (dummyDataDirection)
    dummyData++;
  else
    dummyData--;
}

// Get Sonar data in json style string
String getSonarData(NewPing sonarSensor)
{
  JsonStringBuilder output = JsonStringBuilder(2,6);
  output.add("data", sonarSensor.convert_cm(sonarSensor.ping_median(3)));
  output.add("units", "cm");
  return output.getJsonString();
}

// Get Limit Switch data in json style string
String getLimitSwitchData(int switchPin)
{
  JsonStringBuilder output = JsonStringBuilder(1,5);
  output.add("data", digitalRead(switchPin));
  return output.getJsonString();
}

// Aggregate data into message to be sent to Pi
String getSensorData()
{
  JsonStringBuilder output = JsonStringBuilder(4,12);
  output.add("ack",millis()%2000);

  // This follows the JSON format
  // sensor data: {"data":<data>,"units":<units>}
  // message data: {"ack":<millis>,"sensorName":{sensorData},"sensorName":{sensorData}}\n
  
  //Sonar sensors:
  // sonar
  output.add("sonar1",getSonarData(sonar));

  //Limit switch sensors:
  // LIMITSWITCH1
  output.add("limitSwitch1", getLimitSwitchData(LIMITSWITCH1));

  //DEBUG PLEASE REMOVE
  output.add("dummyData", "{\"data\":"+String(dummyData)+",\"units\":\"dummy\"}");

  //DEBUG PLEASE REMOVE
  if (dummyData%50 == 0)
  {
    output.add("Dumb Chance", "{\"data\":"+String(dummyData)+"}");
  }
  output.add("tick", "{\"data\":\".\"}");

  return output.getJsonString();
}

// Serial input parser
void serialEvent()
{
  // recieve input
  while (Serial.available())
  {
    String readString = Serial.readString();
    incoming += readString;
  }
  incoming.trim();

  // command interpreters
  outgoing = cmdGetSensorData(incoming);

  // reply if desired
  if (outgoing != "")
  {
    Serial.println(outgoing);
    outgoing = "";
  }
  incoming = "";
}

String cmdGetSensorData(String command)
{
  if (command.compareTo("get sensors") == 0)
  {
    return getSensorData();
  }
  return "";
}