#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#pragma pack(1)

enum QueryCommand: uint32_t {
    services = 0,
    routes = 1,
    status = 2,
    outMessages = 3,
    inMessages = 4,
    rTable = 5,
    rTableGW = 6
};

class QueryMessage: public DataMessageGeneric {
public:
    QueryCommand queryCommand;
    uint8_t queryValue;
    String queryAnswer;
    String queryPetition;

    void serialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*) (this))->serialize(doc);

        // Add the derived class data to the JSON object
        
        //doc["flkwsCommand"] = "Test2";
        //doc["query"] = "Test1";
        doc["queryCommand"] = queryCommand;
        doc["query"] = queryPetition;
        doc["queryAns"] = queryAnswer;
        Log.verboseln(F("FF in class QueryMessage: void serialize flkwsCommand %d"),queryCommand );
        Log.verboseln(F("FF in class QueryMessage: void serialize queryvalue %d"),queryValue );
        //JsonObject object = doc.to<JsonObject>();
        //JsonObject object = querydata.createNestedObject();
        //object["query"] = queryvalue;
        
    }

    void deserialize(JsonObject& doc) {
        // Call the base class deserialize function
        ((DataMessageGeneric*) (this))->deserialize(doc);

        // Add the derived class data to the JSON object
        Log.verboseln(F("FF in class QueryMessage: deserialize: seeing local variables in QueryMessage class"));

        queryCommand = doc["queryCommand"];
        Log.verboseln(F("FF in class QueryMessage: void deserialize %d"),queryCommand );
        queryValue = doc["query"];
        Log.verboseln(F("FF in class QueryMessage: void deserialize %d"),queryValue);
    }
};
#pragma pack()
