#include "flkws.h"

static const char* FLKWS_TAG = "FlkwsService";

void Flkws::init() {
    pinMode(LED, OUTPUT);
}

String Flkws::flkwsOn() {
    digitalWrite(LED, LED_ON);
    ESP_LOGV(FLKWS_TAG, "Led On");
    //ESP_LOGV(LED_TAG, "Led On to %X", dst);
    //Log.verboseln(F("Flkws On"));
    return "Flkws On";
}

String Flkws::flkwsOn(uint16_t dst) {
    ESP_LOGV(FLKWS_TAG, "Led On to %X", dst);
    if (dst == LoraMesher::getInstance().getLocalAddress())
        return flkwsOn();

    DataMessage* msg = getFlkwsMessage(FlkwsCommand::POn, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Flkws On";
}

String Flkws::flkwsOff() {
    digitalWrite(LED, LED_OFF);
    //Log.verboseln(F("Flkws Off"));
    ESP_LOGV(FLKWS_TAG, "Led Off");
    return "Flkws Off";
}

String Flkws::flkwsOff(uint16_t dst) {
    ESP_LOGV(FLKWS_TAG, "Led Off to %X", dst);
    if (dst == LoraMesher::getInstance().getLocalAddress())
        return flkwsOff();

    DataMessage* msg = getFlkwsMessage(FlkwsCommand::POff, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Flkws Off";
}

String Flkws::getJSON(DataMessage* message) {
    Log.verboseln(F("FF: Flkws::getJSON"));
    FlkwsMessage* flkwsMessage = (FlkwsMessage*) message;

    StaticJsonDocument<400> doc;  // FF: was 200
    //DynamicJsonDocument doc(400); // FF: do not make dynamic

    JsonObject data = doc.createNestedObject("data");

    flkwsMessage->serialize(data);

    String json;
    serializeJson(doc, json);

    return json;
}

DataMessage* Flkws::getDataMessage(JsonObject data) {
    FlkwsMessage* flkwsMessage = new FlkwsMessage();
    Log.infoln(F("Flkws::getDataMessage new FlkwsMessage()") );
    //Log.infoln(F("FF: Heap after new FlkwsMessage() getFreeHeap() :%d"), ESP.getFreeHeap() );
    Log.infoln(F("FF: Heap after new FlkwsMessage(): %d %d %d %d"),ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());

    flkwsMessage->deserialize(data);  //FF commented.

    flkwsMessage->messageSize = sizeof(FlkwsMessage) - sizeof(DataMessageGeneric);

    return ((DataMessage*) flkwsMessage);
}

DataMessage* Flkws::getFlkwsMessage(FlkwsCommand command, uint16_t dst) {
    FlkwsMessage* flkwsMessage = new FlkwsMessage();
    Log.infoln(F("FF Flkws::getFlkwsMessage: new FlkwsMessage()") );

    flkwsMessage->messageSize = sizeof(FlkwsMessage) - sizeof(DataMessageGeneric);

    flkwsMessage->flkwsCommand = command;

    flkwsMessage->appPortSrc = appPort::FlkwsApp;
    flkwsMessage->appPortDst = appPort::FlkwsApp;

    flkwsMessage->addrSrc = LoraMesher::getInstance().getLocalAddress();
    Log.infoln(F("Flkws::getFlkwsMessage: flkwsMessage->addrSrc: %d"), flkwsMessage->addrSrc );
    flkwsMessage->addrDst = dst;

    return (DataMessage*) flkwsMessage;
}

void Flkws::processReceivedMessage(messagePort port, DataMessage* message) {
    FlkwsMessage* flkwsMessage = (FlkwsMessage*) message;

    Log.infoln(F("FF: Flkws::processReceivedMessage  perform local actions") );

    flkwsCommandS = flkwsMessage->flkwsCommand;   //FF added
    echovalueS = flkwsMessage->echovalue;    //FF added
    
    Log.verboseln(F("FF in Flkws::processReceivedMessage flkwsCommandS %d"),flkwsCommandS );
    Log.verboseln(F("FF in Flkws::processReceivedMessage echovalueS %d"),echovalueS);

    switch (flkwsMessage->flkwsCommand) {
        case FlkwsCommand::POn:
            flkwsOn();
            break;
        case FlkwsCommand::POff:
            flkwsOff();
            break;
        default:
            break;
    }
    
    //delete message;  // FF tried, makes segmentation fault. UPATDE: delete message is in MQTT: void MqttService::processReceivedMessageFromMQTT(String& topic, String& payload) { 
    //delete flkwsMessage; // FF tried, 

    //Flkws& flkws = Flkws::getInstance();
    //flkws.
    Log.infoln(F("FF: Flkws::processReceivedMessage   createAndSendFlkws()") );
    createAndSendFlkws();
}

void Flkws::createAndSendFlkws() {
    // uint8_t metadataSize = 1; //TODO: This should be dynamic with an array of sensors
    // uint16_t metadataSensorSize = metadataSize * sizeof(MetadataSensorMessage);
    uint16_t messageWithHeaderSize = sizeof(FlkwsMessage);// + metadataSensorSize;

    //FlkwsMessage* message = (FlkwsMessage*) malloc(messageWithHeaderSize);
    FlkwsMessage* message = new FlkwsMessage();  // FF: instead of malloc

    Log.verboseln(F("FF Flkws::createAndSendFlkws: Sending flkws message"));

    if (message) {

        message->appPortDst = appPort::MQTTApp;
        message->appPortSrc = appPort::FlkwsApp;
        message->addrSrc = LoraMesher::getInstance().getLocalAddress();
        //Log.verboseln(F("Flkws::createAndSendFlkws message->addrSrc %d"),message->addrSrc);
        message->addrDst = 0;
        message->messageId = flkwsId++;

        message->flkwsCommand = flkwsCommandS;   //FF added
        message->echovalue = echovalueS;    //FF added


        message->messageSize = messageWithHeaderSize - sizeof(DataMessageGeneric);
        //message->gps = GPSService::getInstance().getGPSMessage();
        //message->flSendTimeInterval = FL_UPDATE_DELAY;

        // message->metadataSize = metadataSize;

        //TODO: This should be a vector of sensors
        // Temperature& temperature = Temperature::getInstance();

        // MetadataSensorMessage* tempMetadata = temperature.getMetadataMessage();
        // memcpy(message->sensorMetadata, tempMetadata, sizeof(MetadataSensorMessage));

        // signData(message);

        MessageManager::getInstance().sendMessage(messagePort::MqttPort, (DataMessage*) message);

        //free(message);
        delete message;
    }
    else {
        Log.errorln(F("Failed to allocate memory for flkws message"));
    }
}
