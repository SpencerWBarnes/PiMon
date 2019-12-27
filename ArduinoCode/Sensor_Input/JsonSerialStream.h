// Format to JSON as data is printed to Serial

#pragma once

#include <Arduino.h>

class JsonSerialStream
{
  private:
    bool empty;

    // Most common process, add name to property or nested object
    void addName(String &name);

  public:
    // Open JSON String
    JsonSerialStream();

    // Close JSON String
    void closeMessage();

    // Overload to handle String values
    void addProperty(String &name, String &value);

    // Overload to convert Bool values to true/false instead of 1/0
    void addProperty(String &name, bool value);

    // General property
    template <class T>
    void addProperty(String &name, T value)
    {
        addName(name);
        Serial.print(value);
    }

    // Open nested object, behaves like a new JSON message
    void addNestedObject(String &objectName);

    // Close nested object, but not end of message
    void closeNestedObject();
};