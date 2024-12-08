#include <Arduino.h>

//Configuration
#include "config.h"

//Log
#include "esp_log.h"
#include "esp32-hal-log.h"

//Log Arduino FF: from previous version
#include "ArduinoLog.h"

//Manager
#include "message/messageManager.h"

//LoRaMesh
#include "loramesh/loraMeshService.h"

//WiFi
#include "wifi/wifiServerService.h"

//Wallet  FF: from previous version
#include "wallet/wallet.h"

//Sensors
#include "sensor/sensorService.h"

//Metadata
#include "sensor/metadata/metadata.h"


static const char* TAG = "Main";

// Battery
#pragma region Battery

#include "battery/battery.h"

Battery& battery = Battery::getInstance();

void initBattery() {
    battery.init();
}

#pragma endregion

// Simulator
#pragma region Simulator

#include "simulator/sim.h"

Sim& simulator = Sim::getInstance();

void initSimulator() {
    // Init Simulator
    simulator.init();
}
#pragma endregion

#pragma region Led
#include "led/led.h"

Led& led = Led::getInstance();

void initLed() {
    led.init();
}

#ifdef CDP_ENABLED
#pragma region Cpd
#include "cdp/cdp.h"

Cdp& cdp = Cdp::getInstance();

void initCdp() {
    cdp.init();
}
#endif


#ifdef FLKWS_ENABLED
#pragma region Flksw
#include "flkws/flkws.h"

Flkws& flkws = Flkws::getInstance();

void initFlkws() {
    flkws.init();
}
#endif

#ifdef ECHO_ENABLED
#pragma region Echo
#include "echo/echo.h"

Echo& echo = Echo::getInstance();

void initEcho() {
    echo.init();
}
#endif


#ifdef QUERY_ENABLED
#pragma region Query
#include "query/query.h"

Query& query = Query::getInstance();

void initQuery() {
    query.init();
}
#endif

#ifdef MONITORING_ENABLED
#pragma region Monitoring
#include "monitoring/monitoring.h"

Monitoring& monitoring = Monitoring::getInstance();

void initMonitoring() {
    monitoring.init();
}
#endif

#pragma region Wallet

Wallet& wallet = Wallet::getInstance();

void initWallet() {
    wallet.begin();
}

#pragma endregion


#pragma endregion

#pragma region Metadata

Metadata& metadata = Metadata::getInstance();

void initMetadata() {
    metadata.initMetadata();
}

#pragma endregion

#pragma region Sensors

SensorService& sensorService = SensorService::getInstance();

void initSensors() {
    sensorService.init();
}

#pragma endregion


#pragma region WiFi

WiFiServerService& wiFiService = WiFiServerService::getInstance();

void initWiFi() {
    wiFiService.initWiFi();
}

#pragma endregion

#pragma region LoRaMesher

LoRaMeshService& loraMeshService = LoRaMeshService::getInstance();

void initLoRaMesher() {
    //Init LoRaMesher
    loraMeshService.initLoraMesherService();
}

#pragma endregion


#pragma region MQTT
#include "mqtt/mqttService.h"

MqttService& mqttService = MqttService::getInstance();

void initMQTT() {
    mqttService.initMqtt(String(loraMeshService.getLocalAddress()));
}

#pragma endregion


#pragma region GPS

#include "gps/gpsService.h"

GPSService& gpsService = GPSService::getInstance();

void initGPS() {
    //Initialize GPS
    gpsService.initGPS();
}

#pragma endregion

#ifdef BLUETOOTH_ENABLED
#pragma region SerialBT
#include "bluetooth/bluetoothService.h"
BluetoothService& bluetoothService = BluetoothService::getInstance();

void initBluetooth() {
    bluetoothService.initBluetooth(String(loraMeshService.getLocalAddress(), HEX));
}

#pragma endregion
#endif

#pragma region Manager

MessageManager& manager = MessageManager::getInstance();

void initManager() {
    manager.init();
    ESP_LOGV(TAG, "Manager initialized");

#ifdef BLUETOOTH_ENABLED
    manager.addMessageService(&bluetoothService);
    ESP_LOGV(TAG, "Bluetooth service added to manager");
#endif

#ifdef CDP_ENABLED
    manager.addMessageService(&cdp);
    ESP_LOGV(TAG, "Cdp service added to manager");
#endif

#ifdef FLKWS_ENABLED
    manager.addMessageService(&flkws);
    ESP_LOGV(TAG, "Flkws service added to manager");
#endif

#ifdef QUERY_ENABLED
    manager.addMessageService(&query);
    ESP_LOGV(TAG, "Query service added to manager");
#endif

#ifdef MONITORING_ENABLED
    manager.addMessageService(&monitoring);
    ESP_LOGV(TAG, "Monitoring service added to manager");
#endif

#ifdef ECHO_ENABLED
    manager.addMessageService(&echo);
    ESP_LOGV(TAG, "Echo service added to manager");
#endif

    manager.addMessageService(&gpsService);
    ESP_LOGV(TAG, "GPS service added to manager");

    manager.addMessageService(&loraMeshService);
    ESP_LOGV(TAG, "LoRaMesher service added to manager");

    manager.addMessageService(&wiFiService);
    ESP_LOGV(TAG, "WiFi service added to manager");

    manager.addMessageService(&mqttService);
    ESP_LOGV(TAG, "MQTT service added to manager");

    manager.addMessageService(&led);
    ESP_LOGV(TAG, "Led service added to manager");

    manager.addMessageService(&metadata);
    ESP_LOGV(TAG, "Metadata service added to manager");

    manager.addMessageService(&sensorService);
    ESP_LOGV(TAG, "Sensors service added to manager");

    manager.addMessageService(&simulator);
    ESP_LOGV(TAG, "Simulator service added to manager");

    Serial.println(manager.getAvailableCommands());

    ESP_LOGV(TAG, "All services added to manager");

}

#pragma endregion

// #pragma region AXP20x
// #include <axp20x.h>

// AXP20X_Class axp;

// void initAXP() {
//     Wire.begin(21, 22);
//     if (axp.begin(Wire, AXP192_SLAVE_ADDRESS) == AXP_FAIL) {
//         Serial.println(F("failed to initialize communication with AXP192"));
//     }
//     Serial.println(axp.getBattVoltage());
// }

// float getBatteryVoltage() {
//     return axp.getBattVoltage();
// }

// #pragma endregion

// NEW FOR V1.2

// setup AXP2101/AXP192 power management
#include "XPowersAXP192.tpp"
#include "XPowersAXP2101.tpp"
#include "XPowersLibInterface.hpp"
#define PMU_IRQ 35
#define PMU_WIRE_PORT   Wire
XPowersLibInterface *PMU = NULL;
bool pmuInterrupt;
void setPmuFlag() { pmuInterrupt = true; }
bool initPMU() {
    if (!PMU) {
        PMU = new XPowersAXP2101(PMU_WIRE_PORT);
        if (!PMU->init()) {
            Serial.println("Warning: Failed to find AXP2101 power management");
            ESP_LOGV(TAG, "Warning: Failed to find AXP2101 power management");
            delete PMU;
            PMU = NULL;
        } else {
            Serial.println("AXP2101 PMU init succeeded, using AXP2101 PMU");
            ESP_LOGV(TAG, "AXP2101 PMU init succeeded, using AXP2101 PMU");
        }
    }
    if (!PMU) {
        PMU = new XPowersAXP192(PMU_WIRE_PORT);
        if (!PMU->init()) {
            Serial.println("Warning: Failed to find AXP192 power management");
            ESP_LOGV(TAG, "Warning: Failed to find AXP192 power management");
            delete PMU;
            PMU = NULL;
        } else {
            Serial.println("AXP192 PMU init succeeded, using AXP192 PMU");
            ESP_LOGV(TAG, "AXP192 PMU init succeeded, using AXP192 PMU");
        }
    }
    if (!PMU) {
        return false;
    }
    // PMU->setChargingLedMode(XPOWERS_CHG_LED_BLINK_1HZ);
    pinMode(PMU_IRQ, INPUT_PULLUP);
    attachInterrupt(PMU_IRQ, setPmuFlag, FALLING);
    if (PMU->getChipModel() == XPOWERS_AXP192) {
        PMU->setProtectedChannel(XPOWERS_DCDC3);
        // lora
        PMU->setPowerChannelVoltage(XPOWERS_LDO2, 3300);
        // gps
        PMU->setPowerChannelVoltage(XPOWERS_LDO3, 3300);
        // oled
        PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);

        PMU->enablePowerOutput(XPOWERS_LDO2);
        PMU->enablePowerOutput(XPOWERS_LDO3);

        //protected oled power source
        PMU->setProtectedChannel(XPOWERS_DCDC1);
        //protected esp32 power source
        PMU->setProtectedChannel(XPOWERS_DCDC3);
        // enable oled power
        PMU->enablePowerOutput(XPOWERS_DCDC1);

        //disable not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);

        PMU->disableIRQ(XPOWERS_AXP192_ALL_IRQ);

        PMU->enableIRQ(XPOWERS_AXP192_VBUS_REMOVE_IRQ |
                       XPOWERS_AXP192_VBUS_INSERT_IRQ |
                       XPOWERS_AXP192_BAT_CHG_DONE_IRQ |
                       XPOWERS_AXP192_BAT_CHG_START_IRQ |
                       XPOWERS_AXP192_BAT_REMOVE_IRQ |
                       XPOWERS_AXP192_BAT_INSERT_IRQ |
                       XPOWERS_AXP192_PKEY_SHORT_IRQ
                      );

    } else if (PMU->getChipModel() == XPOWERS_AXP2101) {
      /*The alternative version of T-Beam 1.1 differs from T-Beam V1.1 in that it uses an AXP2101 power chip*/
        //Unuse power channel
        ESP_LOGV(TAG, "AXP2101 PMU detected");
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        PMU->disablePowerOutput(XPOWERS_DCDC3);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_ALDO1);
        PMU->disablePowerOutput(XPOWERS_ALDO4);
        PMU->disablePowerOutput(XPOWERS_BLDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO2);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);

        // GNSS RTC PowerVDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_VBACKUP, 3300);
        PMU->enablePowerOutput(XPOWERS_VBACKUP);

        //ESP32 VDD 3300mV
        // ! No need to set, automatically open , Don't close it
        // PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);
        // PMU->setProtectedChannel(XPOWERS_DCDC1);
        PMU->setProtectedChannel(XPOWERS_DCDC1);

        // LoRa VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        //GNSS VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);
    }

    PMU->enableSystemVoltageMeasure();
    PMU->enableVbusVoltageMeasure();
    PMU->enableBattVoltageMeasure();
    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    PMU->disableTSPinMeasure();

    Serial.printf("=========================================\n");
    if (PMU->isChannelAvailable(XPOWERS_DCDC1)) {
        Serial.printf("DC1  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC1));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC2)) {
        Serial.printf("DC2  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC2));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC3)) {
        Serial.printf("DC3  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC3));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC4)) {
        Serial.printf("DC4  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC4));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC5)) {
        Serial.printf("DC5  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC5)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC5));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO2)) {
        Serial.printf("LDO2 : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_LDO2)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO3)) {
        Serial.printf("LDO3 : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_LDO3)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO1)) {
        Serial.printf("ALDO1: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_ALDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO2)) {
        Serial.printf("ALDO2: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_ALDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO3)) {
        Serial.printf("ALDO3: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_ALDO3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO4)) {
        Serial.printf("ALDO4: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_ALDO4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO4));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO1)) {
        Serial.printf("BLDO1: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_BLDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO2)) {
        Serial.printf("BLDO2: %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_BLDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO2));
    }
    // Serial.printf("=========================================\n");
    // Set the time of pressing the button to turn off
    // PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    // uint8_t opt = PMU->getPowerKeyPressOffTime();
    // Serial.print("PowerKeyPressOffTime:");
    // switch (opt) {
    // case XPOWERS_POWEROFF_4S: Serial.println("4 Second");
    //     break;
    // case XPOWERS_POWEROFF_6S: Serial.println("6 Second");
    //     break;
    // case XPOWERS_POWEROFF_8S: Serial.println("8 Second");
    //     break;
    // case XPOWERS_POWEROFF_10S: Serial.println("10 Second");
    //     break;
    // default:
    //     break;
    // }
    ESP_LOGV(TAG, "finalizing PMU");
    return true;
}

// END NEW FOR V1.2

#ifdef DISPLAY_ENABLED
#pragma region Display
//Display
#include "display.h"

TaskHandle_t display_TaskHandle = NULL;

#define DISPLAY_TASK_DELAY 50 //ms
#define DISPLAY_LINE_TWO_DELAY 10000 //ms
#define DISPLAY_LINE_THREE_DELAY 50000 //ms
#define DISPLAY_LINE_FOUR_DELAY 20000 //ms
#define DISPLAY_LINE_FIVE_DELAY 10000 //ms
#define DISPLAY_LINE_SIX_DELAY 10000 //ms
#define DISPLAY_LINE_ONE 10000 //ms

void display_Task(void* pvParameters) {

    uint32_t lastLineOneUpdate = 0;
    uint32_t lastLineTwoUpdate = 0;
    uint32_t lastLineThreeUpdate = 0;
#ifdef GPS_ENABLED
    uint32_t lastGPSUpdate = 0;
#endif
    uint32_t lastLineFourUpdate = 0;
    uint32_t lastLineFiveUpdate = 0;
    uint32_t lastLineSixUpdate = 0;
    uint32_t lastLineSevenUpdate = 0;

    while (true) {
        //Update line one every DISPLAY_LINE_ONE ms
        if (millis() - lastLineOneUpdate > DISPLAY_LINE_ONE) {
            lastLineOneUpdate = millis();
            // float batteryVoltage = getBatteryVoltage();
            // Given the previous float value, convert it into string with 2 decimal places
            bool isConnected = wiFiService.isConnected() || loraMeshService.hasGateway();
            //bool isConnected = true;
            //String lineOne = "LoRaTRUST-  " + String(isConnected ? "CON" : "NC");
            String lineOne = "LoRaMesher-  " + String(isConnected ? "CON" : "NC");
            Screen.changeLineOne(lineOne);
        }
        //Update line two every DISPLAY_LINE_TWO_DELAY ms
        if (millis() - lastLineTwoUpdate > DISPLAY_LINE_TWO_DELAY) {
            lastLineTwoUpdate = millis();
            String lineTwo = String(loraMeshService.getLocalAddress(), HEX);
            if (wiFiService.isConnected())
                lineTwo += " | " + wiFiService.getIP();
            //if (1)
            //    lineTwo += " | 192.168.43.158";
            Screen.changeLineTwo(lineTwo);
        }
    #ifdef GPS_ENABLED
        //Update line three every DISPLAY_LINE_THREE_DELAY ms
        // if (millis() - lastLineThreeUpdate > DISPLAY_LINE_THREE_DELAY) {
        //     lastLineThreeUpdate = millis();
        //     String lineThree = gpsService.getGPSUpdatedWait();
        //     if (lineThree.begin() != "G")
        //         Screen.changeLineThree(lineThree);
        // }
        //Update GPS every UPDATE_GPS_DELAY ms
        if (millis() - lastGPSUpdate > UPDATE_GPS_DELAY) {
            lastGPSUpdate = millis();
            String lineThree = gpsService.getGPSUpdatedWait();
            if (lineThree.startsWith("G") != 1)
                Screen.changeLineThree(lineThree);
        }
    #endif
        if (millis() - lastLineFourUpdate > DISPLAY_LINE_FOUR_DELAY) {
            lastLineFourUpdate = millis();
            String lineFour; // = "RoutingTable:  ";
            String rt = loraMeshService.getRoutingTable();
            if (1)
                lineFour += rt;
            Screen.changeLineFour(lineFour);
        }

        Screen.drawDisplay();
        vTaskDelay(DISPLAY_TASK_DELAY / portTICK_PERIOD_MS);
    }
}

void createUpdateDisplay() {
    int res = xTaskCreate(
        display_Task,
        "Display Task",
        2048,
        (void*) 1,
        2,
        &display_TaskHandle);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "Display Task creation gave error: %d", res);
        createUpdateDisplay();
    }
}

void initDisplay() {
    Screen.initDisplay();
    createUpdateDisplay();
}
#pragma endregion
#endif

#pragma region Wire

void initWire() {
    Wire.begin((int) I2C_SDA, (int) I2C_SCL);
}

// TODO: The following line could be removed if we add the files in /src to /lib. However, at this moment, it generates a lot of errors
// TODO: https://docs.platformio.org/en/stable/advanced/unit-testing/structure/shared-code.html#unit-testing-shared-code

#ifndef PIO_UNIT_TESTING

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Set log level
    // esp_log_level_set("*", ESP_LOG_VERBOSE);

    ESP_LOGV(TAG, "Build environment name: %s", BUILD_ENV_NAME);

    // Initialize Wire
    initWire();

    ESP_LOGV(TAG, "Heap before initManager: %d", ESP.getFreeHeap());

    // Initialize Manager
    initManager();

    ESP_LOGV(TAG, "Heap after initManager: %d", ESP.getFreeHeap());

     ESP_LOGV(TAG, "Init PMU") ;
    if (initPMU()) {
      ESP_LOGV(TAG, "Init PMU OK") ;
    } else {
      ESP_LOGV(TAG, "Init PMU FAILED") ;
    }
    
#ifdef WIFI_ENABLED
    // Initialize WiFi
    initWiFi();
    ESP_LOGV(TAG, "Heap after initWiFi: %d", ESP.getFreeHeap());
#endif

#ifdef MQTT_ENABLED
    // Initialize MQTT
    initMQTT();
    ESP_LOGV(TAG, "Heap after initMQTT: %d", ESP.getFreeHeap());
#endif

    // Initialize AXP192
    // initAXP();

#ifdef GPS_ENABLED
    // Initialize GPS
    initGPS();
    ESP_LOGV(TAG, "Heap after initGPS: %d", ESP.getFreeHeap());
#endif

    // Initialize LoRaMesh
    initLoRaMesher();
    ESP_LOGV(TAG, "Heap after initLoRaMesher: %d", ESP.getFreeHeap());

#ifdef BLUETOOTH_ENABLED
    // Initialize Bluetooth
    initBluetooth();
    ESP_LOGV(TAG, "Heap after initBluetooth: %d", ESP.getFreeHeap());
#endif

#ifdef LED_ENABLED
    // Initialize Led
    initLed();
#endif

#ifdef CDP_ENABLED
    // Initialize Cdp
    initCdp();
    Log.verboseln("Heap after initCdp: %d", ESP.getFreeHeap());
#endif

#ifdef MONITORING_ENABLED
    // Initialize Monitoring 
    initMonitoring();
    Log.verboseln("Heap after initMonitoring: %d", ESP.getFreeHeap());
#endif


#ifdef METADATA_ENABLED
    // Initialize Metadata
    initMetadata();
    ESP_LOGV(TAG, "Heap after initMetadata: %d", ESP.getFreeHeap());
#endif

#ifdef SENSORS_ENABLED
    // Initialize Sensors
    initSensors();
    ESP_LOGV(TAG, "Heap after initSensors: %d", ESP.getFreeHeap());
#endif

#ifdef DISPLAY_ENABLED
    // Initialize Display
    initDisplay();
    ESP_LOGV(TAG, "Heap after initDisplay: %d", ESP.getFreeHeap());
#endif


#ifdef SIMULATION_ENABLED
    // Initialize Simulator
    initSimulator();
    ESP_LOGV(TAG, "Heap after initSimulator: %d", ESP.getFreeHeap());
#endif

#ifdef BATTERY_ENABLED
    // Initialize Battery
    initBattery();
    ESP_LOGV(TAG, "Heap after initBattery: %d", ESP.getFreeHeap());
#endif

    ESP_LOGV(TAG, "Setup finished");

#ifdef LED_ENABLED
    // Blink 2 times to show that the device is ready
    led.ledBlink();
#endif
}

void loop() {
    vTaskDelay(200000 / portTICK_PERIOD_MS);

    Serial.printf("FREE HEAP: %d\n", ESP.getFreeHeap());
    Serial.printf("Min, Max: %d, %d\n", ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());

#ifdef BATTERY_ENABLED
    if (battery.getVoltagePercentage() < 20) {
        ESP_LOGE(TAG, "Battery is low, deep sleeping for %d s", DEEP_SLEEP_TIME);
        mqttService.disconnect();
        wiFiService.disconnectWiFi();
        esp_wifi_deinit();

        ESP.deepSleep(DEEP_SLEEP_TIME * (uint32_t) 1000000);
    }
#endif

    if (ESP.getFreeHeap() < 40000) {
        ESP_LOGE(TAG, "Not enough memory to process mqtt messages");
        ESP.restart();
        return;
    }

    if (millis() > 21600000) {
        ESP_LOGE(TAG, "Restarting device to avoid memory leaks");
        ESP.restart();
    }
    }

#endif