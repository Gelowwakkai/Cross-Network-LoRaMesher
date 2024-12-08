#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include <ArduinoJson.h>

#include "helpers/helper.h"

#include "wallet/wallet.h"

#pragma pack(1)

class SignatureMessage {
public:
    uint8_t signature[ETHERS_SIGNATURE_LENGTH]; // Stored as hexadecimal uint8_t array

    void serializeSignature(JsonObject& doc) {
        // Add the derived class data to the JSON object
        doc["signature"] = "0x" + Helper::Helper::uint8ArrayToHexString(signature, ETHERS_SIGNATURE_LENGTH);
    }

};

#pragma pack()