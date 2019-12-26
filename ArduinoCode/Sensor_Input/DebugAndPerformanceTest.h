// Contain all debugging code and performance testing

#pragma once

#include <Arduino.h>
#include "JsonStringBuilder.h"

//DEBUG Library: Used for measuring Heap Fragmentation
#include "MemoryInfo.h"

//DEBUG Variables: Dummy variables for timer performance and testing logs
extern unsigned int dummyData;
extern unsigned long avgDummyTime;
extern unsigned long dummyStart;

// To be run in loop()
//DEBUG Function: any necessary dummy variable changes
void testCode();

// To be run in serialEvent
//DEBUG Function: Get various performance statistics
void getPerformanceData(JsonStringBuilder &outgoing);

// To be run in serialEvent
//DEBUG Function: Get various performance statistics
void getTestData(JsonStringBuilder &outgoing);