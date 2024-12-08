#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#include "gps/gpsMessage.h"
#include "message/dataMessage.h"
#include "sensorlto/signatureMessage.h"

#pragma pack(1)

//static const char* CPD_TAG = "CpdService";

enum CdpState: uint8_t {
    Idle = 0,
    Active = 1
};

class CdpMessage: public DataMessageGeneric{
    public:
    CdpState cdpState;
    int cdpSendTimeInterval;
    GPSMessage gps;
    uint32_t helloPacket[5] = {0, 1, 2, 3, 4};
    String uuid = "143584a5-8c4d-4477-82ec-a5025b5fde65";
    String timestamp = "2023-10-25T07:43:16.499Z";
    String TicVersion = "2023.0091";
    String id = "RTG001";
    String name = "RTG01";
    String type = "RTG";
    String brand = "Konecranes";
    String number = "01";
    String family =  "RTG01-15";
    uint32_t REAL22 = 9.04800033569336;
    uint32_t REAL30 = 7.489452362060547;

    void serialize(JsonObject& doc) {
        //ESP_LOGV(CDP_TAG, "in class CdpMessage: void serialize");
        int i;
        ((DataMessageGeneric*)(this))->serialize(doc);
        doc["cdpState"] = 1; // FF: for short monitoring message
/*
        int arrSize = sizeof(helloPacket)/4;
        JsonArray cranedata = doc.createNestedArray("cranedata");

        JsonObject object = cranedata.createNestedObject();
        object["uuid"] = "143584a5-8c4d-4477-82ec-a5025b5fde65";
        object["timestamp"] = "2023-10-25T07:43:16.499Z";
        object["TicVersion"] = "2023.0091";
        object["id"] = "RTG001";
        object["name"] = "RTG01";
        object["type"] = "RTG";
        object["brand"] = "Konecranes";
        object["number"] = "01";
        object["family"] =  "RTG01-15";
        object["REAL22"] = 9.04800033569336;
        object["REAL30"] = 7.489452362060547;
        object["DB920,X12.3"] = false;
        object["DB920,X1.6"] = false;
        object["D920,X234.4"]  = false;
*/
        /* v1 cdp was till here */
/*        object["D920,X34.3"]  = false;
        object["D920,X173.4"]  = true;
        object["D920,REAL178"]  = 5.946649074554443;
        object["D920,REAL194"]  = -0.06960000097751617;
        object["D920,X3.1"]  = false;
        object["D920,X198.0"]  = false;
        object["D920,X198.1"]  = false;
        object["D920,X198.4"]  = false;
        object["D920,X204.0"]  = false;
        object["D920,REAL156"]  = 0;
        object["D920,X233.0"]  = false;
        object["D920,X225.3"]  = false;
        object["D920,X225.4"]  = false;
        object["D920,X225.5"]  = false;
        object["D920,X225.7"]  = false;
        object["D920,X226.1"]  = false;
        object["D920,X226.2"]  = false;
        object["D920,X230.3"]  = false;
        object["D920,X230.6"]  = true;
        object["D920,X226.0"]  = false;
        object["D920,INT228"]  = 3;
        object["D920,X224.3"]  = false;
        object["D920,X224.4"]  = false;
        object["D920,X224.7"]  = true;
        object["D920,X225.2"]  = false;
        object["D920,X231.6"]  = false;
        object["D920,X232.0"]  = false;
        object["D920,X231.4"]  = false;
        object["D920,X232.4"]  = false;
        object["D920,X232.7"]  = false;
        object["D920,X234.6"]  = false;
        object["D920,X235.4"]  = false;
        object["D920,X240.1"]  = false;
        object["D920,X240.2"]  = false;
        object["D920,X246.2"]  = false;
        object["D920,X256.0"]  = false;
        object["D920,X2.2"]  = false;
        object["D920,X2.5"]  = false;
        object["D920,X140.0"]  = true;
        object["D920,REAL242"]  = 14.333000183105469;
        object["D920,X278.0"]  = false;
        object["D920,X278.1"]  = false;
        object["D920,X2.4"]  = false;
        object["D920,X173.4"]  = true;
        object["D920,X198.0"]  = false;
        object["D920,X198.1"]  = false;
        object["D920,X230.1"]  = false;
        object["D920,X3.5"]  = true;
        object["D920,REAL280"]  = 2.4990875720977783;
        object["D920,X278.2"]  = false;
        object["D920,X173.1"]  = false;
        object["D321,REAL4"]  = 0;
        object["D920,X310.0"]  = true;
        object["D920,X310.1"]  = false;
        object["D920,X310.2"]  = false;
        
        float gpsdata[2] = {35.81637191772461, 14.53260612487793};
        JsonArray measurements = doc.createNestedArray("Y:DB591,R354;X:DB591,R35");
        for(i=0;i<2;i++){
            measurements.add(gpsdata[i]);
        }
    */
        //JsonArray measurements = doc.createNestedArray("measurements");
        //for(i=0;i<arrSize;i++){
        //    measurements.add(helloPacket[i]);
        //}
        //JsonArray values = doc.createNestedArray("values");
        //for(i=0;i<arrSize;i++){
        //    values.add(i);
        //}
    
    }

    void deserialize(JsonObject& doc) {
        ((DataMessageGeneric*)(this))->deserialize(doc);
        cdpState = doc["cdpState"];
        Log.verbose(F("Cdp in cdpMessage.h deserialized %d"), cdpState);
    }
};
#pragma pack()

