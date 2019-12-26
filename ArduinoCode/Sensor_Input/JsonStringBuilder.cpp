// Simplify the creation, transport, and modification of JSON strings
#include "JsonStringBuilder.h"

JsonStringBuilder::JsonStringBuilder(int estimatedNumberOfElements, int estimatedElementSize)
{
    message = "";
    message.reserve((estimatedElementSize+4)*estimatedNumberOfElements);
}

// Overload of template for string data, needs quotes \"
void JsonStringBuilder::add(String propertyName, String propertyValue)
{
    message += "\""+ propertyName +"\":\""+ propertyValue +"\",";
}

// Overload of template for other JSON builders, uses getJsonString and no quotes
void JsonStringBuilder::add(String propertyName, JsonStringBuilder propertyValue)
{
    message += "\""+ propertyName +"\":"+ propertyValue.getJsonString() +",";
}

// Overload of template for bool data, write as true/false not 1/0
void JsonStringBuilder::add(String propertyName, bool propertyValue)
{
    message += "\""+ propertyName +"\":"+ String(propertyValue?"true":"false") +",";
}

String JsonStringBuilder::getJsonString()
{
    // Cut off last comma , and encompass in braces {}
    return "{"+ message.substring(0, message.length()-1) +"}";
}

bool JsonStringBuilder::empty()
{
    return message == "";
}