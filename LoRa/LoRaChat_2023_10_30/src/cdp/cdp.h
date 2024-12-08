#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/messageService.h"

#include "message/messageManager.h"

#include "cdp/cdpMessage.h"

#include "cdp/cdpCommandService.h"

#include "config.h"

#include "wallet/wallet.h"  // FF: needed if signature is made

class Cdp: public MessageService{
    public:
    static Cdp& getInstance(){
        static Cdp instance;
        return instance;
    }

    void init();
    void startCdp();
    void stopCdp();

    // Cdp commands
    String cdpIdle();
    String cdpIdle(uint16_t dst);
    String cdpActive();
    String cdpActive(uint16_t dst);

    // sending
    void createAndSendCdp();

    // receiving
    String getJSON(DataMessage* message);
    DataMessage* getDataMessage(JsonObject data);
    DataMessage* getCdpMessage(CdpState state, uint16_t dst);
    void processReceivedMessage(messagePort port, DataMessage* message);

    //var
    CdpCommandService* cdpCommandService = new CdpCommandService();

    private:
    Cdp(): MessageService(CdpApp, "Cdp") {
        commandService = cdpCommandService;
    };

    //var
    TaskHandle_t cdp_TaskHandle = NULL;
    bool running = false;
    uint8_t cdpId = 0;

    void createCdpTask();

    static void cdpLoop(void*);

    void signData(CdpMessage *message);
     
    void getJSONDataObject(JsonObject& doc, CdpMessage* cdpdataMessage);

    void getJSONSignObject(JsonObject& doc, CdpMessage* cdpdataMessage);

};