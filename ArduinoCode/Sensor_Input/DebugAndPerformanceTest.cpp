// Contain all debugging code and performance testing

#include "DebugAndPerformanceTest.h"

//DEBUG Variables: Dummy variables for timer performance and testing logs
unsigned int dummyData = 0;
unsigned long avgDummyTime = 0;
unsigned long dummyStart = 0;

// To be run in loop()
//DEBUG Function: any necessary dummy variable changes
void testCode()
{
    if (dummyData >= 60000)
    {
        avgDummyTime = ((avgDummyTime*3) + (millis() - dummyStart))/4;
        dummyData = 0;
        dummyStart = millis();
    }
    dummyData++;
}

// To be run in serialEvent
//DEBUG Function: Get various performance statistics
void getPerformanceData(JsonStringBuilder &outgoing)
{
  // Show avg time to count dummyData
  outgoing.add("avgDummyTime", "{\"data\":"+String(avgDummyTime)+",\"units\":\"ms\"}");
  // Show heap fragmentation
  outgoing.add("Heap Fragmentation", "{\"data\":"+String(getFragmentation())+",\"units\":\"%\"}");
}

// To be run in serialEvent
//DEBUG Function: Get various performance statistics
void getTestData(JsonStringBuilder &outgoing)
{
  outgoing.add("tick", String("."));
  if (dummyData % 50 == 0) {
    outgoing.add("Dumb Chance", String(dummyData));
  }
}