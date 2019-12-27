// This library was written as a Software Engineering Senior Project for 
//  the CSE Department of Mississippi State University
// Authors: Spencer Barnes, Jordan Stremming, Tyler Whiticker, Jake Griesmer

// Provide functions to assist in the recieving of data and logs from an 
//  Arduino board. To be used in conjunction with JsonSerial Stream library
// This library is intended to be expanded upon to handle more data types.

#pragma once

#include <Arduino.h>
#include <NewPing.h>
#include "JsonSerialStream.h"

class Monitor
{
  private:
    // Large string to store logs
    String logBook;

    // Size of reserved logBook space
    int logBookCapacity;

    // Add a segment from the log book to passed in value, segments are 
    //  separated by \v. Returns the index after the segment's terminating
    //  character \v
    unsigned int getLogSegment(String &segment, unsigned int i, unsigned int stop);

  public:
    Monitor(int logSize);

    // Add sonar data object from New Ping ultrasonic sensors to Stream
    // {"data":<int>, "units":"cm"}
    void getSonarObject(NewPing sonarSensor, JsonSerialStream &outgoing);

    // Returns value of Arduino Pin wired to limit switch
    bool getLimitSwitchData(int switchPin);

    // Add log comment to log book, adding two segments
    bool log(String logStream, String data);

    // Add all log comments to the Stream 
    void getLogs(JsonSerialStream &outgoing);
};