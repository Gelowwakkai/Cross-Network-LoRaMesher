#include "cdp.h"
//#include "sensorlto/temperature.h"
#include "sensor.h"

static const char* CPD_TAG = "CpdService";

void Cdp::init(){
    //Log.notice(F("Initializing cdp" CR));
    ESP_LOGV(CPD_TAG, "Initializing cdp");
    createCdpTask();
    startCdp();
}

void Cdp::startCdp(){
    //Log.notice(F("Starting cdp" CR));
    ESP_LOGV(CPD_TAG, "Starting cdp");
    running = true;
    xTaskNotifyGive(cdp_TaskHandle);
    //Log.noticeln(F("Cdp task started"));
    ESP_LOGV(CPD_TAG, "Cdp task started");
}

void Cdp::stopCdp() {
    //Log.notice(F("Stopping cdp" CR));
    ESP_LOGV(CPD_TAG, "Stopping cdp");
    running = false;
}

//commands for Cpd

String Cdp::cdpIdle(){
    digitalWrite(LED, LED_OFF);
    //Log.verboseln(F("Cdp waiting"));
    ESP_LOGV(CPD_TAG, "Cdp waiting");
    return "Cdp waiting";
}

String Cdp::cdpIdle(uint16_t dst) {
    if (dst == LoraMesher::getInstance().getLocalAddress()) { //FF: falta {}?? -> added
        //Log.verboseln(F("return to cdpIdle()"));
        ESP_LOGV(CPD_TAG, "return to cdpIdle()");
        return cdpIdle();
    } 

    DataMessage* msg = getCdpMessage(CdpState::Idle, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Cdp waiting";
}

String Cdp::cdpActive(){
    digitalWrite(LED, LED_ON);
    //Log.verboseln(F("Cdp active"));
    ESP_LOGV(CPD_TAG, "Cdp active");
    return "Cdp active";
}

String Cdp::cdpActive(uint16_t dst) {
    if (dst == LoraMesher::getInstance().getLocalAddress())  { //FF: falta {}?? -> added //FF: falta {}??
        return cdpActive();
    }

    DataMessage* msg = getCdpMessage(CdpState::Active, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Cdp active";
}

//end commands for Cpd

// for receiving

DataMessage* Cdp::getDataMessage(JsonObject data) {
    CdpMessage* cdpMessage = new CdpMessage();
    cdpMessage->deserialize(data);
    cdpMessage->messageSize = sizeof(CdpMessage) - sizeof(DataMessageGeneric);  //FF: substracts header
    //Log.verbose(F("Cdp getDataMessage in cdp.cpp"));
    ESP_LOGV(CPD_TAG, "Cdp getDataMessage in cdp.cpp");
    return ((DataMessage*) cdpMessage);
}

DataMessage* Cdp::getCdpMessage(CdpState state, uint16_t dst){
    CdpMessage* cdpMessage = new CdpMessage();
    cdpMessage-> messageSize = sizeof(CdpMessage) - sizeof(DataMessageGeneric);
    cdpMessage->cdpState = state;
    cdpMessage->appPortSrc = appPort::CdpApp;
    cdpMessage->appPortDst = appPort::CdpApp;
    cdpMessage->addrSrc = LoraMesher::getInstance().getLocalAddress();
    cdpMessage->addrDst = dst;
    //Log.verbose(F("Cdp getCdpMessage in cdp.cpp %d"), cdpMessage->cdpState);
    ESP_LOGV(CPD_TAG, "Cdp getCdpMessage in cdp.cpp %d", cdpMessage->cdpState);
    return (DataMessage*) cdpMessage;
}

void Cdp::processReceivedMessage(messagePort port, DataMessage* message){
    CdpMessage* cdpMessage = (CdpMessage*) message;
    switch(cdpMessage->cdpState){
        case CdpState::Idle:
            //Log.verbose(F("Cdp processReveivedMessage :Idle"));
            ESP_LOGV(CPD_TAG, "Cdp getCdpMessage in cdp.cpp");
            cdpIdle();
        case CdpState::Active:
            //Log.verbose(F("Cdp processReveivedMessage: Active"));
            ESP_LOGV(CPD_TAG, "Cdp processReveivedMessage: Active");
            cdpActive();
        default:
            //Log.verboseln(F("Cdp processReceivedMessage :default"));
            //Log.verboseln(F("Cdp processReceivedMessage :default value: %d"), cdpMessage->cdpState);
            ESP_LOGV(CPD_TAG, "Cdp processReceivedMessage :default value %d", cdpMessage->cdpState);
            break;        
    }
}


String Cdp::getJSON(DataMessage* message){
    //Log.verbose(F("Cdp getJSON" CR));
    ESP_LOGV(CPD_TAG, "Cdp getJSON");
    CdpMessage* cdpMessage = (CdpMessage*) message;
    //DynamicJsonDocument doc(1024);
    DynamicJsonDocument doc(2048);
    JsonObject jsonObj = doc.to<JsonObject>();
    JsonObject dataObj = jsonObj.createNestedObject("data");

    getJSONDataObject(dataObj, cdpMessage);

    //serializeJsonPretty(doc, Serial); // FF: uncommented

    // getJSONSignObject(jsonObj, metadataMessage);

    String json;
    serializeJson(doc, json);
    //serializeJsonPretty(doc, Serial);  // FF: added

    return json;
}

// implementation of private functions

void Cdp::createCdpTask(){
    int res = xTaskCreate(
        cdpLoop,
        "Cdp Task",
        4096,
        (void*) 1,
        2,
        &cdp_TaskHandle
    );

    if (res != pdPASS) {
        //Log.error(F("Failed to create cdp task" CR));
        ESP_LOGV(CPD_TAG, "Failed to create cdp task");
    }
}

void Cdp::cdpLoop(void* pvParameters) {
    Cdp& cdp =Cdp::getInstance();

    while (true){
        if (!cdp.running) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        } 
        else {
            vTaskDelay(CDP_UPDATE_DELAY / portTICK_PERIOD_MS);
            //Log.verboseln(F("before send cdpLoop Free heap: %d"), ESP.getFreeHeap());
            ESP_LOGV(CPD_TAG, "before send cdpLoop Free heap: %d", ESP.getFreeHeap());
            cdp.createAndSendCdp();
            //Log.verboseln(F("after send cdpLoop Free heap: %d"), ESP.getFreeHeap());
            ESP_LOGV(CPD_TAG, "after send cdpLoop Free heap: %d", ESP.getFreeHeap());
        }
    }
}

void Cdp::createAndSendCdp(){
    // uint8_t metadataSize = 1; //TODO: This should be dynamic with an array of sensors
    // uint16_t metadataSensorSize = metadataSize * sizeof(MetadataSensorMessage);

    uint16_t messageWithHeaderSize = sizeof(CdpMessage);  // + metadataSensorSize;

    CdpMessage* message = (CdpMessage*) malloc(messageWithHeaderSize);

    if (message) {
        //Log.verboseln(F("in createAndSendCdp() cdp.cpp Sending cdp message"));
        ESP_LOGV(CPD_TAG, "in createAndSendCdp() cdp.cpp Sending cdp message");

        message->appPortDst = appPort::MQTTApp;
        message->appPortSrc = appPort::CdpApp;
        message->addrSrc = LoraMesher::getInstance().getLocalAddress();
        message->addrDst = 0;
        message->messageId = cdpId++;

        message->messageSize = messageWithHeaderSize - sizeof(DataMessageGeneric);
        //message->gps = GPSService::getInstance().getGPSMessage();
        message->cdpSendTimeInterval = CDP_UPDATE_DELAY;

        // message->metadataSize = metadataSize;

        //TODO: This should be a vector of sensors
        // Temperature& temperature = Temperature::getInstance();

        // MetadataSensorMessage* tempMetadata = temperature.getMetadataMessage();
        // memcpy(message->sensorMetadata, tempMetadata, sizeof(MetadataSensorMessage));

        // signData(message);

        MessageManager::getInstance().sendMessage(messagePort::MqttPort, (DataMessage*) message);

        free(message);
    }
    else {
        //Log.errorln(F("Failed to allocate memory for cps message"));
        ESP_LOGV(CPD_TAG, "Failed to allocate memory for cps message");
    }
}

void Cdp::signData(CdpMessage* cdpMessage) {
    DynamicJsonDocument doc(1024);
    JsonObject jsonObj = doc.to<JsonObject>();
    getJSONDataObject(jsonObj, cdpMessage);

    // Print the object to the serial port
    // serializeJsonPretty(doc, Serial);

    String json;
    serializeJson(doc, json);

    // Wallet::getInstance().signJson(json, metadataMessage->signature);
    // memcpy(metadataMessage->signature, signStr.c_str(), signStr.length());
}

void Cdp::getJSONDataObject(JsonObject& doc, CdpMessage* cdpMessage) {
    cdpMessage->serialize(doc);
}

void Cdp::getJSONSignObject(JsonObject& doc, CdpMessage* cdpMessage) {
    // metadataMessage->serializeSignature(doc);
}
