// This library was written as a Software Engineering Senior Project for 
//  the CSE Department of Mississippi State University
// Authors: Spencer Barnes, Jordan Stremming, Tyler Whiticker, Jake Griesmer

// Provide functions to assist in the recieving of data and logs from an 
//  Arduino board. To be used in conjunction with a JsonStream library such
//  as JsonSerialStream.

#include "Logger.h"

Logger::Logger(int logBookSize)
{
  // Reserve large but not lethal size for logs
  logBook.reserve(logBookSize);
  logBook = "";
  logBookCapacity = logBookSize;
}

// Add log comment to log book, adding two segments with error checking
// void logUnsafe(S logStream, String &data)
// void logUnsafe(S logStream, T data)
// bool log(S logStream, T data)

// Add log comment to log book, adding two segments with error checking
bool Logger::log(String logStream, String data)
{
  // if dangerously long
  if (logBook.length() + logStream.length() + logStream.length() >= logBookCapacity)
    return false;

  logBook += logStream;
  logBook += '\v'; // This is a vertical tab, a completely unused char
  logBook += data;
  logBook += '\v';
  return true;
}

// Add all log comments to the Stream 
void Logger::getLogs(JsonSerialStream &outgoing)
{
  unsigned int logLen = logBook.length();
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
  // Clear log book
  logBook = "";
}

// Add a segment from the log book to passed in value, segments are 
//  separated by \v. Returns the index after the segment's terminating
//  character \v
unsigned int Logger::getLogSegment(String &segment, unsigned int i, unsigned int stop)
{
  char currentChar;
  while (i < stop)
  {
    currentChar = logBook.charAt(i);
    if (currentChar == '\v')
      return ++i;
    segment += currentChar;
    i++;
  }
  return i;
} 