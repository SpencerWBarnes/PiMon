// Contain all debugging code and performance testing

#pragma once

#include <Arduino.h>
#include "JsonSerialStream.h"
#include "Logger.h"

//Grab global Logger
extern Logger logger;

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
void getPerformanceData(JsonSerialStream &outgoing);

// To be run in serialEvent
//DEBUG Function: Get timing statistics
void getTimingData(JsonSerialStream &outgoing);

// To be run in serialEvent
//DEBUG Function: Get various RAM statistics
void getMemoryData(JsonSerialStream &outgoing);

// To be run in serialEvent
//DEBUG Function: Get add test or dummy data
void getTestData(JsonSerialStream &outgoing);