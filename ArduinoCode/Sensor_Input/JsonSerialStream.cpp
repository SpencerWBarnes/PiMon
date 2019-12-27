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

#include "JsonSerialStream.h"

// Open JSON String
JsonSerialStream::JsonSerialStream()
{
    empty = true;
    Serial.print(F("{"));
}

// This class has many template classes that are implemented in the header
// void addName(S name)
// void addStringValue(S value)
// void addProperty(S name, String &value)
// void addProperty(S name, const char value[])
// void addProperty(S name, bool value)
// void addProperty(S name, T value)
// void addPropertyAsString(S name, T value)
// void addNestedObject(S objectName)

// Close JSON String
void JsonSerialStream::closeMessage()
{
    // Newline \n indicates end of message
    Serial.print(F("}\n"));
}

// Close nested object, but not end of message
void JsonSerialStream::closeNestedObject()
{
    Serial.print(F("}"));
    empty = false;
}