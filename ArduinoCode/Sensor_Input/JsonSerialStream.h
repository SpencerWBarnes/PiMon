// Format to JSON as data is printed to Serial

// This class was written to transfer information off the Arduino using the
//  JavaScript Object Notation (JSON) standard. This causes us to send 
//  extra characters, take the time to format the data, and this class 
//  takes executable space, but this standardization will improve usability
//  on reciever's side as many libraries exist for many langauges to 
//  "rehydrate" JSON strings into variables again.

// This class makes extensive use of template functions. These are 
//  functions in which a datatype is variable. This class makes use of this 
//  in order to make functions independent of input type, essentially 
//  acting as an overload for any datatype, and instead the work of sending
//  the data by Serial is put upon .print() and not me.

// Please note:
//  - template functions are required to be implemented at declaration
//  - the use of F() is a macro to save RAM
//  - the use of & is Pass by Reference, saves RAM and time by giving the  
//      actual String rather than a copy of the String
//  - this class does not have proper format error checking

// Prevents this file from being loaded multiple times
#pragma once

#include <Arduino.h>

class JsonSerialStream
{
  private:
    bool empty;

    // Most common process, add name to property or nested object
    template <class S>
    void addName(S name)
    {
        // Prevent comma for first element 
        if (empty)
        {
            Serial.print(F("\""));
            empty = false;
        }
        else
            Serial.print(F(",\""));

        Serial.print(name);
        Serial.print(F("\":"));
    }

    // Handle types that are JavaScript strings: ex. String, char[], char
    template <class S>
    void addStringValue(S value)
    {
        // Add quotes for string
        Serial.print(F("\""));
        Serial.print(value);
        Serial.print(F("\""));
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
        addStringValue(value);
    }

    // Overload to handle string literals (const char*) values
    template <class S>
    void addProperty(S name, const char value[])
    {
        addName(name);
        addStringValue(value);
    }

    // Overload to convert Bool values to true/false instead of 1/0
    template <class S>
    void addProperty(S name, bool value)
    {
        addName(name);
        // Ternary operator to simplify process
        Serial.print( (value) ? F("true") : F("false") );
    }

    // General property
    template <class S, class T>
    void addProperty(S name, T value)
    {
        addName(name);
        Serial.print(value);
    }

    // Let user force data to be sent as string data
    template <class S, class T>
    void addPropertyAsString(S name, T value)
    {
        addName(name);
        addStringValue(value);
    }

    // Open nested object, behaves like a new JSON message
    template <class S>
    void addNestedObject(S objectName)
    {
        addName(objectName);
        Serial.print(F("{"));
        // Behave as if empty for interior properties
        empty = true;
    }

    // Close nested object, but not end of message
    void closeNestedObject();
};