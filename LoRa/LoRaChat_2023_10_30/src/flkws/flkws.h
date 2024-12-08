#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "sensor.h"

#include "message/messageService.h"

#include "message/messageManager.h"

#include "flkwsCommandService.h"

#include "flkwsMessage.h"

#include "config.h"

#include "LoraMesher.h"

class Flkws : public MessageService {
public:
    /**
     * @brief Construct a new GPSService object
     *
     */
    static Flkws& getInstance() {
        static Flkws instance;
        return instance;
    }

    FlkwsCommandService* flkwsCommandService = new FlkwsCommandService();

    void init();

    String flkwsOn();

    String flkwsOn(uint16_t dst);

    String flkwsOff();

    String flkwsOff(uint16_t dst);

    void createAndSendFlkws();

    String getJSON(DataMessage* message);

    DataMessage* getDataMessage(JsonObject data);

    DataMessage* getFlkwsMessage(FlkwsCommand command, uint16_t dst);

    void processReceivedMessage(messagePort port, DataMessage* message);

private:
    Flkws() : MessageService(FlkwsApp, "Flkws") {
        commandService = flkwsCommandService;
    };

    uint8_t flkwsId = 0;

    FlkwsCommand flkwsCommandS;
    u_int32_t echovalueS;

};

    
