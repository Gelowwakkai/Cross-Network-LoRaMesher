#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#include "gps/gpsMessage.h"
#include "message/dataMessage.h"
#include "sensorlto/signatureMessage.h"

#pragma pack(1)

// static const char* MONITORING_TAG = "MonitoringService";

enum MonitoringState: uint8_t {
    mIdle = 0,
    mActive = 1
};

class MonitoringMessage: public DataMessageGeneric{
    public:
    MonitoringState monitoringState;
    int monitoringSendTimeInterval;
    uint16_t nAddress;
    uint16_t nServices;
    uint16_t nRoutes;
    String routeTable;
    uint16_t ledStatus;
    uint16_t outMessages;
    uint16_t inMessages;
    float sensorValue;
    //uint16_t sensorValue;
    GPSMessage gps;
    
    uint32_t helloPacket[5] = {0, 1, 2, 3, 4};

    void serialize(JsonObject& doc) {
        //ESP_LOGV(MONITORING_TAG, "in class MonitoringMessage: void serialize");
        int i;
        ((DataMessageGeneric*)(this))->serialize(doc);
        doc["monitoringState"] = 1; // FF: for short monitoring message
        doc["nServices"] = nServices;
        doc["nRoutes"] = nRoutes;
        doc["ledStatus"] = ledStatus;
        doc["outMessages"] = outMessages;
        doc["inMessages"] = inMessages;
        doc["sensorValue"] = sensorValue;
    }

    void deserialize(JsonObject& doc) {
        ((DataMessageGeneric*)(this))->deserialize(doc);
        monitoringState = doc["monitoringState"];
        Log.verbose(F("Monitoring in monitoringMessage.h deserialized %d"), monitoringState);
    }
};
#pragma pack()

