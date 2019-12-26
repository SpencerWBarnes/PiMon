// Simplify the creation, transport, and modification of JSON strings

#pragma once

#include <Arduino.h>

class JsonStringBuilder
{
  private:
    String message;

  public:
    JsonStringBuilder(int estimatedNumberOfElements, int estimatedElementSize);

    // Overload of template for string data, needs quotes \"
    void add(String propertyName, String propertyValue);

    // Overload of template for other JSON builders, uses getJsonString and no quotes
    void add(String propertyName, JsonStringBuilder propertyValue);

    // Overload of template for bool data, write as true/false not 1/0
    void add(String propertyName, bool propertyValue);

    template <class T>
    void add(String propertyName, T propertyValue)
    {
        // Name requires quotes, thus they must be escaped as \"
        message += "\""+ propertyName +"\":"+ propertyValue +",";
    }

    String getJsonString();

    bool empty();
};