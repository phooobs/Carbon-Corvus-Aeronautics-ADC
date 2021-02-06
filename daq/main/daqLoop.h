/* 
    This task is for aquiring raw data and saving it to a sd card
    as well as making the data available to the rest of the program.
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "daqPacket.h"

// SD card connections
    // GPIO 15 -- CMD -- 10k pullup
    // GPIO 14 -- CLK
    // GPIO 2 -- GPIO 0 -- D0 -- 10k pullup
    // D3 -- 10k pullup

void daqLoop (void * pvParameters) {
    struct DAQPacket daqPacket;

    // 1-line SD card init
    static const char *TAG = "SD card";
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;

    // Set internal GPIO pullups for SD card pins
    gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY); // CMD
    gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY); // D0

    // Options for mounting the filesystem.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // Try to mount SD card at /sd
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sd", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        vTaskDelete(NULL); // kill this task
    }

    daqPacket.testValue = 0;
    for (;;) { // loop  forever

        // test code
        daqPacket.testValue++;
        vTaskDelay(10);

        // opening and closing the file like this may be slow

        // write data to SC card
        // Create and open file
        // note there is a file name length limit
        FILE* sd_card_file = fopen("/sd/w_t.raw", "a");
        if (sd_card_file == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            vTaskDelete(NULL); // kill this task
        }
        fprintf(sd_card_file, "%i\n", daqPacket.testValue);            
        fclose(sd_card_file);

        *(struct DAQPacket*)pvParameters = daqPacket; // send data in packet back to main
    }
    
    // Unmount SD card
    esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "Card unmounted");
}