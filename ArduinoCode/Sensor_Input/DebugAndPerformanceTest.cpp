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
void getPerformanceData(JsonSerialStream &outgoing)
{
  getTimingData(outgoing);
  getMemoryData(outgoing);
}

// To be run in serialEvent
//DEBUG Function: Get timing statistics
void getTimingData(JsonSerialStream &outgoing)
{
  // Show avg time to count dummyData
  outgoing.addNestedObject("avgDummyTime");
  outgoing.addProperty("data", avgDummyTime);
  outgoing.addProperty("units", "ms");
  outgoing.closeNestedObject();
}

// To be run in serialEvent
//DEBUG Function: Get various RAM statistics
void getMemoryData(JsonSerialStream &outgoing)
{
  // Show heap fragmentation
  outgoing.addNestedObject("Heap Fragmentation");
  outgoing.addProperty("data", getFragmentation());
  outgoing.addProperty("units", "%");
  outgoing.closeNestedObject();

  // Show Free memory
  outgoing.addNestedObject("Free memory");
  outgoing.addProperty("data", getTotalAvailableMemory());
  outgoing.addProperty("units", "bytes");
  outgoing.closeNestedObject();
}

// To be run in serialEvent
//DEBUG Function: Get various performance statistics
void getTestData(JsonSerialStream &outgoing)
{
  if (dummyData % 50 == 0) {
    outgoing.addPropertyAsString("Dumb Chance", dummyData);
  }
}