/* 
    Entry point for the aircraft's daq and control system
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "daqPacket.h"
#include "daqLoop.h"
#include "systemLoop.h"

void app_main(void)
{
    printf("Carbon Corvus Aeronautics DAQ Startup\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);
    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    struct DAQPacket daqPacket;
    
    // use uxTaskGetStackHighWaterMark() to figure out how big the stack sizes should actualy be
    printf("Launching system task on core 0\n");
    xTaskCreatePinnedToCore(systemLoop, "System Task", 10000, (void*)&daqPacket, 1, NULL, 0);
    printf("Launching DAQ task on core 1\n");
    xTaskCreatePinnedToCore(daqLoop, "DAQ Task", 10000, (void*)&daqPacket, 10, NULL, 1);
}
