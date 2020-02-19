// This library was written as a Software Engineering Senior Project for 
//  the CSE Department of Mississippi State University
// Authors: Spencer Barnes, Jordan Stremming, Tyler Whiticker, Jake Griesmer

// Provide functions to assist in the recieving of data and logs from an 
//  Arduino board. To be used in conjunction with JsonSerial Stream library
// This library is intended to be expanded upon to handle more data types.

#pragma once

#include <Arduino.h>
#include "JsonSerialStream.h"

class Logger
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
    Logger(int logSize);

    // Add log comment to log book, adding two segments without checking 
    //  the length of the log (Possibly beyond log book capacity)
    // Saves some RAM
    template <class S>
    void logUnsafe(S logStream, String &data)
    {
      logBook += logStream;
      logBook += '\v'; // This is a vertical tab, a completely unused char
      logBook += data;
      logBook += '\v';
    }

    // Add log comment to log book, adding two segments without checking 
    //  the length of the log (Possibly beyond log book capacity)
    // Saves some RAM
    template <class S, class T>
    void logUnsafe(S logStream, T data)
    {
      logBook += logStream;
      logBook += '\v'; // This is a vertical tab, a completely unused char
      logBook += data;
      logBook += '\v';
    }

    // Add log comment to log book, adding two segments with error checking
    bool log(String logStream, String data);

    // Add log comment to log book, adding two segments with error checking
    template <class S, class T>
    bool log(S logStream, T data)
    {
      return log(String(logStream),String(data));
    }

    // Add all log comments to the Stream 
    void getLogs(JsonSerialStream &outgoing);
};