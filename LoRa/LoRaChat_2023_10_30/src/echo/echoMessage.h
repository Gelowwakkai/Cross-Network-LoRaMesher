#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#pragma pack(1)

enum EchoCommand: uint8_t {
    POff = 0,
    POn = 1
};

class EchoMessage: public DataMessageGeneric {
public:
    EchoCommand echoCommand;
    u_int32_t echovalue;

    void serialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*) (this))->serialize(doc);

        // Add the derived class data to the JSON object
        
        //doc["flkwsCommand"] = "Test2";
        //doc["echo"] = "Test1";
        doc["echoCommand"] = echoCommand;
        doc["echo"] = echovalue;
        Log.verboseln(F("FF in class EchoMessage: void serialize flkwsCommand %d"),echoCommand );
        Log.verboseln(F("FF in class EchoMessage: void serialize echovalue %d"),echovalue );
        //JsonObject object = doc.to<JsonObject>();
        //JsonObject object = echodata.createNestedObject();
        //object["echo"] = echovalue;
        
    }

    void deserialize(JsonObject& doc) {
        // Call the base class deserialize function
        ((DataMessageGeneric*) (this))->deserialize(doc);

        // Add the derived class data to the JSON object
        Log.verboseln(F("FF in class EchoMessage: deserialize: seeing local variables in EchoMessage class"));

        echoCommand = doc["echoCommand"];
        Log.verboseln(F("FF in class EchoMessage: void deserialize %d"),echoCommand );
        echovalue = doc["echo"];
        Log.verboseln(F("FF in class EchoMessage: void deserialize %d"),echovalue);
    }
};
#pragma pack()
