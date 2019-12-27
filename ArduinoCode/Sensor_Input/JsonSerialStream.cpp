// Format to JSON as data is printed to Serial
#include "JsonSerialStream.h"

// Open JSON String
JsonSerialStream::JsonSerialStream()
{
    empty = true;
    Serial.print("{");
}

// Close JSON String
void JsonSerialStream::closeMessage()
{
    // Newline \n indicates end of message
    Serial.print("}\n");
}

// Most common process, add name to property or nested object
void JsonSerialStream::addName(String &name)
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

// Overload to handle String values
void JsonSerialStream::addProperty(String &name, String &value)
{
    addName(name);
    // Add quotes for Strings
    Serial.print("\"");
    Serial.print(value);
    Serial.print("\"");
}

// Overload to convert Bool values to true/false instead of 1/0
void JsonSerialStream::addProperty(String &name, bool value)
{
    addName(name);
    // Ternary operator to simplify process
    Serial.print( (value) ? "true" : "false" );
}

// Open nested object, behaves like a new JSON message
void JsonSerialStream::addNestedObject(String &objectName)
{
    addName(objectName);
    Serial.print("{");
    // Behave as if empty for interior properties
    empty = true;
}

// Close nested object, but not end of message
void JsonSerialStream::closeNestedObject()
{
    Serial.print("}");
    empty = false;
}