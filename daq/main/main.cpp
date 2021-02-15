#include "Arduino.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "daqPacket.h"
#include "daqLoop.h"
#include "systemLoop.h"

extern "C" void app_main()
{
    initArduino();
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    // Do your own thing

    ESP_LOGI("app_main", "Carbon Corvus Aeronautics DAQ Startup");

    struct DAQPacket daqPacket;
    
    // use uxTaskGetStackHighWaterMark() to figure out how big the stack sizes should actualy be
    ESP_LOGI("app_main", "Launching system task on core 0");
    xTaskCreatePinnedToCore(systemLoop, "System Task", 10000, (void*)&daqPacket, 1, NULL, 0);
    ESP_LOGI("app_main", "Launching DAQ task on core 1");
    xTaskCreatePinnedToCore(daqLoop, "DAQ Task", 10000, (void*)&daqPacket, 10, NULL, 1);
    ESP_LOGI("app_main", "Launching kill DAQ in 10 task on core 0");
    xTaskCreatePinnedToCore(killDaqLoopDelay, "DAQ Task", 10000, NULL, 2, NULL, 0);
}