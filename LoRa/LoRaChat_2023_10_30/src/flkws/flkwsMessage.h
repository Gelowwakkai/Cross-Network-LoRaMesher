#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#pragma pack(1)

enum FlkwsCommand: uint8_t {
    POff = 0,
    POn = 1
};

class FlkwsMessage: public DataMessageGeneric {
public:
    FlkwsCommand flkwsCommand;
    u_int32_t echovalue;

    void serialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*) (this))->serialize(doc);

        // Add the derived class data to the JSON object
        
        //doc["flkwsCommand"] = "Test2";
        //doc["echo"] = "Test1";
        doc["flkwsCommand"] = flkwsCommand;
        doc["echo"] = echovalue;
        Log.verboseln(F("FF in class FlkwsMessage: void serialize flkwsCommand %d"),flkwsCommand );
        Log.verboseln(F("FF in class FlkwsMessage: void serialize echovalue %d"),echovalue );
        //JsonObject object = doc.to<JsonObject>();
        //JsonObject object = echodata.createNestedObject();
        //object["echo"] = echovalue;
        
    }

    void deserialize(JsonObject& doc) {
        // Call the base class deserialize function
        ((DataMessageGeneric*) (this))->deserialize(doc);

        // Add the derived class data to the JSON object
        Log.verboseln(F("FF in class FlkwsMessage: deserialize: seeing local variables in FlkwsMessage class"));

        flkwsCommand = doc["flkwsCommand"];
        Log.verboseln(F("FF in class FlkwsMessage: void deserialize %d"),flkwsCommand );
        echovalue = doc["echo"];
        Log.verboseln(F("FF in class FlkwsMessage: void deserialize %d"),echovalue);
    }
};
#pragma pack()
