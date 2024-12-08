#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "sensor.h"

#include "message/messageService.h"

#include "message/messageManager.h"

#include "echoCommandService.h"

#include "echoMessage.h"

#include "config.h"

#include "LoraMesher.h"

class Echo : public MessageService {
public:
    /**
     * @brief Construct a new GPSService object
     *
     */
    static Echo& getInstance() {
        static Echo instance;
        return instance;
    }

    EchoCommandService* echoCommandService = new EchoCommandService();

    void init();

    String echoOn();

    String echoOn(uint16_t dst);

    String echoOff();

    String echoOff(uint16_t dst);

    void createAndSendEcho();

    String getJSON(DataMessage* message);

    DataMessage* getDataMessage(JsonObject data);

    DataMessage* getEchoMessage(EchoCommand command, uint16_t dst);

    void processReceivedMessage(messagePort port, DataMessage* message);

private:
    Echo() : MessageService(EchoApp, "Echo") {
        commandService = echoCommandService;
    };

    uint8_t echoId = 0;

    EchoCommand echoCommandS;
    u_int32_t echovalueS;

};

    
