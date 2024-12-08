#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include <DallasTemperature.h>

#include <OneWire.h>

#include "message/messageService.h"

#include "message/messageManager.h"

#include "monitoring/monitoringMessage.h"

#include "monitoring/monitoringCommandService.h"

#include "config.h"

#include "led/led.h"

#include "query/query.h"

#include "wallet/wallet.h"  // FF: needed if signature is made

class Monitoring: public MessageService{
    public:
    static Monitoring& getInstance(){
        static Monitoring instance;
        return instance;
    }

    void init();
    void startMonitoring();
    void stopMonitoring();

    // Monitoring commands
    String monitoringIdle();
    String monitoringIdle(uint16_t dst);
    String monitoringActive();
    String monitoringActive(uint16_t dst);

    int getOutMessages();
    
    float getSensorValue();

    // sending
    void createAndSendMonitoring();

    // receiving
    String getJSON(DataMessage* message);
    DataMessage* getDataMessage(JsonObject data);
    DataMessage* getMonitoringMessage(MonitoringState state, uint16_t dst);
    void processReceivedMessage(messagePort port, DataMessage* message);

    //var
    MonitoringCommandService* monitoringCommandService = new MonitoringCommandService();
    RouteNode *rtn;

    private:
    Monitoring(): MessageService(MonitoringApp, "Monitoring") {
        commandService = monitoringCommandService;
    };

    //var
    TaskHandle_t monitoring_TaskHandle = NULL;
    bool running = false;
    uint8_t monitoringId = 0;
    Led& led = Led::getInstance();
    
    void createMonitoringTask();

    static void monitoringLoop(void*);

    void signData(MonitoringMessage *message);
     
    void getJSONDataObject(JsonObject& doc, MonitoringMessage* monitoringdataMessage);

    void getJSONSignObject(JsonObject& doc, MonitoringMessage* monitoringdataMessage);
    
    int getServices();

    String getRoutingTable();

    int getRoutes();

    int getLEDstatus();

    int getInMessages();
};