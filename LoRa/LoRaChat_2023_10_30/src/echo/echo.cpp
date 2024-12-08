#include "echo.h"

static const char* ECHO_TAG = "EchoService";

void Echo::init() {
    pinMode(LED, OUTPUT);
}

String Echo::echoOn() {
    digitalWrite(LED, LED_ON);
    ESP_LOGV(ECHO_TAG, "Led On");
    //ESP_LOGV(LED_TAG, "Led On to %X", dst);
    //Log.verboseln(F("Echo On"));
    return "Echo On";
}

String Echo::echoOn(uint16_t dst) {
    ESP_LOGV(ECHO_TAG, "Led On to %X", dst);
    if (dst == LoraMesher::getInstance().getLocalAddress())
        return echoOn();

    DataMessage* msg = getEchoMessage(EchoCommand::POn, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Echo On";
}

String Echo::echoOff() {
    digitalWrite(LED, LED_OFF);
    //Log.verboseln(F("Echo Off"));
    ESP_LOGV(ECHO_TAG, "Led Off");
    return "Echo Off";
}

String Echo::echoOff(uint16_t dst) {
    ESP_LOGV(ECHO_TAG, "Led Off to %X", dst);
    if (dst == LoraMesher::getInstance().getLocalAddress())
        return echoOff();

    DataMessage* msg = getEchoMessage(EchoCommand::POff, dst);
    MessageManager::getInstance().sendMessage(messagePort::LoRaMeshPort, msg);

    delete msg;

    return "Echo Off";
}

String Echo::getJSON(DataMessage* message) {
    Log.verboseln(F("FF: Echo::getJSON"));
    EchoMessage* echoMessage = (EchoMessage*) message;

    StaticJsonDocument<400> doc;  // FF: was 200
    //DynamicJsonDocument doc(400); // FF: do not make dynamic

    JsonObject data = doc.createNestedObject("data");

    echoMessage->serialize(data);

    String json;
    serializeJson(doc, json);

    return json;
}

DataMessage* Echo::getDataMessage(JsonObject data) {
    EchoMessage* echoMessage = new EchoMessage();
    Log.infoln(F("Echo::getDataMessage new EchoMessage()") );
    //Log.infoln(F("FF: Heap after new EchoMessage() getFreeHeap() :%d"), ESP.getFreeHeap() );
    Log.infoln(F("FF: Heap after new EchoMessage(): %d %d %d %d"),ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());

    echoMessage->deserialize(data);  //FF commented.

    echoMessage->messageSize = sizeof(EchoMessage) - sizeof(DataMessageGeneric);

    return ((DataMessage*) echoMessage);
}

DataMessage* Echo::getEchoMessage(EchoCommand command, uint16_t dst) {
    EchoMessage* echoMessage = new EchoMessage();
    Log.infoln(F("FF Echo::getEchoMessage: new EchoMessage()") );

    echoMessage->messageSize = sizeof(EchoMessage) - sizeof(DataMessageGeneric);

    echoMessage->echoCommand = command;

    echoMessage->appPortSrc = appPort::EchoApp;
    echoMessage->appPortDst = appPort::EchoApp;

    echoMessage->addrSrc = LoraMesher::getInstance().getLocalAddress();
    Log.infoln(F("Echo::getEchoMessage: echoMessage->addrSrc: %d"), echoMessage->addrSrc );
    echoMessage->addrDst = dst;

    return (DataMessage*) echoMessage;
}

void Echo::processReceivedMessage(messagePort port, DataMessage* message) {
    EchoMessage* echoMessage = (EchoMessage*) message;

    Log.infoln(F("FF: Echo::processReceivedMessage  perform local actions") );

    echoCommandS = echoMessage->echoCommand;   //FF added
    echovalueS = echoMessage->echovalue;    //FF added
    
    Log.verboseln(F("FF in Echo::processReceivedMessage echoCommandS %d"),echoCommandS );
    Log.verboseln(F("FF in Echo::processReceivedMessage echovalueS %d"),echovalueS);

    switch (echoMessage->echoCommand) {
        case EchoCommand::POn:
            echoOn();
            break;
        case EchoCommand::POff:
            echoOff();
            break;
        default:
            break;
    }
    
    //delete message;  // FF tried, makes segmentation fault. UPATDE: delete message is in MQTT: void MqttService::processReceivedMessageFromMQTT(String& topic, String& payload) { 
    //delete echoMessage; // FF tried, 

    //Echo& echo = Echo::getInstance();
    //echo.
    Log.infoln(F("FF: Echo::processReceivedMessage   createAndSendEcho()") );
    createAndSendEcho();
}

void Echo::createAndSendEcho() {
    // uint8_t metadataSize = 1; //TODO: This should be dynamic with an array of sensors
    // uint16_t metadataSensorSize = metadataSize * sizeof(MetadataSensorMessage);
    uint16_t messageWithHeaderSize = sizeof(EchoMessage);// + metadataSensorSize;

    //EchoMessage* message = (EchoMessage*) malloc(messageWithHeaderSize);
    EchoMessage* message = new EchoMessage();  // FF: instead of malloc

    Log.verboseln(F("FF Echo::createAndSendEcho: Sending echo message"));

    if (message) {

        message->appPortDst = appPort::MQTTApp;
        message->appPortSrc = appPort::EchoApp;
        message->addrSrc = LoraMesher::getInstance().getLocalAddress();
        //Log.verboseln(F("Echo::createAndSendEcho message->addrSrc %d"),message->addrSrc);
        message->addrDst = 0;
        message->messageId = echoId++;

        message->echoCommand = echoCommandS;   //FF added
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
        Log.errorln(F("Failed to allocate memory for echo message"));
    }
}
