// Format to JSON as data is printed to Serial
#include "JsonSerialStream.h"

// Open JSON String
JsonSerialStream::JsonSerialStream()
{
    empty = true;
    Serial.print("{");
}

// This class has many template classes that are implemented in the header


// Close JSON String
void JsonSerialStream::closeMessage()
{
    // Newline \n indicates end of message
    Serial.print("}\n");
}

// Close nested object, but not end of message
void JsonSerialStream::closeNestedObject()
{
    Serial.print("}");
    empty = false;
}