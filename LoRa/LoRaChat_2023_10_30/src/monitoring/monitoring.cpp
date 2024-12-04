#include "monitoring.h"
//#include "sensorlto/temperature.h"
#include "sensor.h"

static const char* MONITORING_TAG = "MonitoringService";

void Monitoring::init(){
    //Log.notice(F("Initializing monitoring" CR));
    ESP_LOGV(MONITORING_TAG, "Initializing monitoring");
    createMonitoringTask();
    startMonitoring();
}

void Monitoring::startMonitoring(){
    //Log.notice(F("Starting monitoring" CR));
    ESP_LOGV(MONITORING_TAG, "Starting monitoring");
    running = true;
    xTaskNotifyGive(monitoring_TaskHandle);
    //Log.noticeln(F("Monitoring task started"));
    ESP_LOGV(MONITORING_TAG, "Monitoring task started");
}

void Monitoring::stopMonitoring() {
    //Log.notice(F("Stopping monitoring" CR));
    ESP_LOGV(MONITORING_TAG, "Stopping monitoring");
    running = false;
}

//commands for Cpd

String Monitoring::monitoringIdle(){
    digitalWrite(LED, LED_OFF);
    //Log.verboseln(F("Monitoring waiting"));
    ESP_LOGV(MONITORING_TAG, "Monitoring waiting");
    return "Monitoring waiting";
}

String Monitoring::monitoringIdle(uint16_t dst) {
    if (dst == LoraMesher::getInstance().getLocalAddress()) { //FF: falta {}?? -> added
        //Log.verboseln(F("return to monitoringIdle()"));
        ESP_LOGV(MONITORING_TAG, "return to monitoringIdle()");
        return monitoringIdle();
    } 

    DataMessage* msg = getMonitoringMessage(MonitoringState::mIdle, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Monitoring waiting";
}

String Monitoring::monitoringActive(){
    digitalWrite(LED, LED_ON);
    //Log.verboseln(F("Monitoring active"));
    ESP_LOGV(MONITORING_TAG, "Monitoring active");
    return "Monitoring active";
}

String Monitoring::monitoringActive(uint16_t dst) {
    if (dst == LoraMesher::getInstance().getLocalAddress())  { //FF: falta {}?? -> added //FF: falta {}??
        return monitoringActive();
    }

    DataMessage* msg = getMonitoringMessage(MonitoringState::mActive, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Monitoring active";
}

//end commands for Cpd

// for receiving

DataMessage* Monitoring::getDataMessage(JsonObject data) {
    MonitoringMessage* monitoringMessage = new MonitoringMessage();
    monitoringMessage->deserialize(data);
    monitoringMessage->messageSize = sizeof(MonitoringMessage) - sizeof(DataMessageGeneric);  //FF: substracts header
    //Log.verbose(F("Monitoring getDataMessage in monitoring.cpp"));
    ESP_LOGV(MONITORING_TAG, "Monitoring getDataMessage in monitoring.cpp");
    return ((DataMessage*) monitoringMessage);
}

DataMessage* Monitoring::getMonitoringMessage(MonitoringState state, uint16_t dst){
    MonitoringMessage* monitoringMessage = new MonitoringMessage();
    monitoringMessage-> messageSize = sizeof(MonitoringMessage) - sizeof(DataMessageGeneric);
    monitoringMessage->monitoringState = state;
    monitoringMessage->appPortSrc = appPort::MonitoringApp;
    monitoringMessage->appPortDst = appPort::MonitoringApp;
    monitoringMessage->addrSrc = LoraMesher::getInstance().getLocalAddress();
    monitoringMessage->addrDst = dst;
    //Log.verbose(F("Monitoring getMonitoringMessage in monitoring.cpp %d"), monitoringMessage->monitoringState);
    ESP_LOGV(MONITORING_TAG, "Monitoring getMonitoringMessage in monitoring.cpp %d", monitoringMessage->monitoringState);
    return (DataMessage*) monitoringMessage;
}

void Monitoring::processReceivedMessage(messagePort port, DataMessage* message){
    MonitoringMessage* monitoringMessage = (MonitoringMessage*) message;
    switch(monitoringMessage->monitoringState){
        case MonitoringState::mIdle:
            //Log.verbose(F("Monitoring processReveivedMessage :Idle"));
            ESP_LOGV(MONITORING_TAG, "Monitoring getMonitoringMessage in monitoring.cpp");
            monitoringIdle();
        case MonitoringState::mActive:
            //Log.verbose(F("Monitoring processReveivedMessage: Active"));
            ESP_LOGV(MONITORING_TAG, "Monitoring processReveivedMessage: Active");
            monitoringActive();
        default:
            //Log.verboseln(F("Monitoring processReceivedMessage :default"));
            //Log.verboseln(F("Monitoring processReceivedMessage :default value: %d"), monitoringMessage->monitoringState);
            ESP_LOGV(MONITORING_TAG, "Monitoring processReceivedMessage :default value %d", monitoringMessage->monitoringState);
            break;        
    }
}


String Monitoring::getJSON(DataMessage* message){
    //Log.verbose(F("Monitoring getJSON" CR));
    ESP_LOGV(MONITORING_TAG, "Monitoring getJSON");
    MonitoringMessage* monitoringMessage = (MonitoringMessage*) message;
    //DynamicJsonDocument doc(1024);
    DynamicJsonDocument doc(2048);
    JsonObject jsonObj = doc.to<JsonObject>();
    JsonObject dataObj = jsonObj.createNestedObject("data");

    getJSONDataObject(dataObj, monitoringMessage);

    //serializeJsonPretty(doc, Serial); // FF: uncommented

    // getJSONSignObject(jsonObj, metadataMessage);

    String json;
    serializeJson(doc, json);
    //serializeJsonPretty(doc, Serial);  // FF: added

    return json;
}

// implementation of private functions

void Monitoring::createMonitoringTask(){
    int res = xTaskCreate(
        monitoringLoop,
        "Monitoring Task",
        4096,
        (void*) 1,
        2,
        &monitoring_TaskHandle
    );

    if (res != pdPASS) {
        //Log.error(F("Failed to create monitoring task" CR));
        ESP_LOGV(MONITORING_TAG, "Failed to create monitoring task");
    }
}

void Monitoring::monitoringLoop(void* pvParameters) {
    Monitoring& monitoring =Monitoring::getInstance();

    while (true){
        if (!monitoring.running) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        } 
        else {
            vTaskDelay(MONITORING_UPDATE_DELAY / portTICK_PERIOD_MS);
            //Log.verboseln(F("before send monitoringLoop Free heap: %d"), ESP.getFreeHeap());
            ESP_LOGV(MONITORING_TAG, "before send monitoringLoop Free heap: %d", ESP.getFreeHeap());
            monitoring.createAndSendMonitoring();
            //Log.verboseln(F("after send monitoringLoop Free heap: %d"), ESP.getFreeHeap());
            ESP_LOGV(MONITORING_TAG, "after send monitoringLoop Free heap: %d", ESP.getFreeHeap());
        }
    }
}

void Monitoring::createAndSendMonitoring(){
    // uint8_t metadataSize = 1; //TODO: This should be dynamic with an array of sensors
    // uint16_t metadataSensorSize = metadataSize * sizeof(MetadataSensorMessage);

    uint16_t messageWithHeaderSize = sizeof(MonitoringMessage);  // + metadataSensorSize;

    MonitoringMessage* message = (MonitoringMessage*) malloc(messageWithHeaderSize);

    if (message) {
        //Log.verboseln(F("in createAndSendMonitoring() monitoring.cpp Sending monitoring message"));
        ESP_LOGV(MONITORING_TAG, "in createAndSendMonitoring() monitoring.cpp Sending monitoring message");

        message->appPortDst = appPort::MQTTApp;
        message->appPortSrc = appPort::MonitoringApp;
        message->addrSrc = LoraMesher::getInstance().getLocalAddress();
        message->addrDst = 0;
        message->messageId = monitoringId++;
        message->messageSize = messageWithHeaderSize - sizeof(DataMessageGeneric);
        message->monitoringSendTimeInterval = MONITORING_UPDATE_DELAY;
        message->nServices = getServices();
        message->nRoutes = getRoutes();
        message->routeTable = getRoutingTable();
        message->ledStatus = getLEDstatus();
        message->outMessages = getOutMessages();
        message->inMessages = getInMessages();
        MessageManager::getInstance().sendMessage(messagePort::MqttPort, (DataMessage*) message);

        free(message);
    }
    else {
        //Log.errorln(F("Failed to allocate memory for cps message"));
        ESP_LOGV(MONITORING_TAG, "Failed to allocate memory for cps message");
    }
}

void Monitoring::signData(MonitoringMessage* monitoringMessage) {
    DynamicJsonDocument doc(1024);
    JsonObject jsonObj = doc.to<JsonObject>();
    getJSONDataObject(jsonObj, monitoringMessage);

    // Print the object to the serial port
    // serializeJsonPretty(doc, Serial);

    String json;
    serializeJson(doc, json);

    // Wallet::getInstance().signJson(json, metadataMessage->signature);
    // memcpy(metadataMessage->signature, signStr.c_str(), signStr.length());
}

void Monitoring::getJSONDataObject(JsonObject& doc, MonitoringMessage* monitoringMessage) {
    monitoringMessage->serialize(doc);
}

void Monitoring::getJSONSignObject(JsonObject& doc, MonitoringMessage* monitoringMessage) {
    // metadataMessage->serializeSignature(doc);
}

int Monitoring::getServices(){
    MessageManager& manager = MessageManager::getInstance();
    return manager.getActiveServices().size();
}


String Monitoring::getRoutingTable(){
    LM_LinkedList<RouteNode>* routingTableList = LoRaMeshService::getInstance().radio.routingTableListCopy();
    int thisNodeAdr = LoraMesher::getInstance().getLocalAddress();
    String routeTable = "No routes";
    routingTableList->setInUse();
    if(routingTableList->getLength() > 0){
        RouteNode *rtn = routingTableList->getCurrent();
        routeTable =  "" + rtn->networkNode.address;
        routeTable =  routeTable + " (" + rtn->networkNode.metric + ") via " + rtn->via;
    }
    while (routingTableList->next()){
        RouteNode *rtn = routingTableList->getCurrent();
        String auxNode = rtn->networkNode.address + " (" + rtn->networkNode.metric;
        auxNode = auxNode + + ") via " + rtn->via;
        routeTable = routeTable + "," + auxNode;
    }
    return routeTable;
}

int Monitoring::getRoutes(){
    LM_LinkedList<RouteNode>* routingTableList = LoRaMeshService::getInstance().radio.routingTableListCopy();
    return routingTableList->getLength();
}

int Monitoring::getLEDstatus(){
    return led.getState();
}

int Monitoring::getOutMessages(){
    Query& query = Query::getInstance();
    return query.getQueryID()+monitoringId;
}

int Monitoring::getInMessages(){
    Query& query = Query::getInstance();
    return query.getInQuery();
}
