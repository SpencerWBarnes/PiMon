// Format to JSON as data is printed to Serial

#pragma once

#include <Arduino.h>

class JsonSerialStream
{
  private:
    bool empty;

    // Please note that template functions are required to be implemented at declaration
    // Most common process, add name to property or nested object
    template <class S>
    void addName(S name)
    {
        // Prevent comma for first element 
        if (empty)
        {
            Serial.print("\"");
            empty = false;
        }
        else
            Serial.print(",\"");

        Serial.print(name);
        Serial.print("\":");
    }

  public:
    // Open JSON String
    JsonSerialStream();

    // Close JSON String
    void closeMessage();

    // Overload to handle String values
    template <class S>
    void addProperty(S name, String &value)
    {
        addName(name);
        // Add quotes for Strings
        Serial.print("\"");
        Serial.print(value);
        Serial.print("\"");
    }

    // Overload to handle string literals (const char*) values
    template <class S>
    void addProperty(S name, const char value[])
    {
        addName(name);
        // Add quotes for Strings
        Serial.print("\"");
        Serial.print(value);
        Serial.print("\"");
    }

    // Overload to convert Bool values to true/false instead of 1/0
    template <class S>
    void addProperty(S name, bool value)
    {
        addName(name);
        // Ternary operator to simplify process
        Serial.print( (value) ? "true" : "false" );
    }

    // General property
    template <class S, class T>
    void addProperty(S name, T value)
    {
        addName(name);
        Serial.print(value);
    }

    // Open nested object, behaves like a new JSON message
    template <class S>
    void addNestedObject(S objectName)
    {
        addName(objectName);
        Serial.print("{");
        // Behave as if empty for interior properties
        empty = true;
    }

    // Close nested object, but not end of message
    void closeNestedObject();
};