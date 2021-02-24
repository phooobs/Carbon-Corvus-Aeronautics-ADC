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
#include <Wire.h>

#include "daqPacket.h"

// SD card connections
// GPIO 15 -- CMD -- 10k pullup
// GPIO 14 -- CLK
// GPIO 2 -- GPIO 0 -- D0 -- 10k pullup
// D3 -- 10k pullup

// Airspeed sensor connections
// GPIO 22 -- SCL -- 10k pullup
// GPIO 21 -- SDA -- 10k pullup

#define MS4525DO_ADDRESS 0x28

bool aquireData;

void daqLoop(void *pvParameters) {

  // start I2C for airspeed sensor
  Wire.begin();

  DAQPacket daqPacket;

  // 1-line SD card init
  ESP_LOGI("daqLoop", "Attempting to mount SDMMC peripheral at /sd");
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 1;

  // Set internal GPIO pullups for SD card pins
  gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY); // CMD
  gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);  // D0

  // Options for mounting the filesystem.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  // Try to mount SD card at /sd
  sdmmc_card_t *card;
  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sd", &host, &slot_config, &mount_config, &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE("daqLoop", "Failed to mount filesystem. "
                          "If you want the card to be formatted, set format_if_mount_failed = true.");
    } else {
      ESP_LOGE("daqLoop", "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    vTaskDelete(NULL); // kill this task
  }

  // Create and open file
  // note there is a file name length limit
  FILE *sd_card_file = fopen("/sd/w_t.raw", "w");
  if (sd_card_file == NULL) {
    ESP_LOGE("daqLoop", "Failed to open file for writing");
    vTaskDelete(NULL); // kill this task
  }

  ESP_LOGI("daqLoop", "Card mounted and file openend");
  
  daqPacket.testValue = 0;
  aquireData = true;
  while (aquireData) { // loop untill told not to

    // test code
    daqPacket.testValue++;

    // request and read 4 bytes from airspeed sensor
    #define MS4525DO_BYTES 4
    Wire.requestFrom(MS4525DO_ADDRESS, MS4525DO_BYTES, true);
    if (Wire.available() == MS4525DO_BYTES) {
      for (int i = 0; i < MS4525DO_BYTES; i++) {
        daqPacket.ms4525doData[i] = Wire.read();
      }
    }

    // write bytes to SD card
    putc(0xff, sd_card_file); // delimiter byte 0xff
    uint32_t timeBytes = esp_timer_get_time();
    putc((uint8_t)(timeBytes >> 24), sd_card_file); // timeing bytes in microsecconds
    putc((uint8_t)(timeBytes >> 16), sd_card_file);
    putc((uint8_t)(timeBytes >> 8), sd_card_file);
    putc((uint8_t)(timeBytes >> 0), sd_card_file);
    // write ms4525do data
    for (int i = 0; i < MS4525DO_BYTES; i++) {
      putc(daqPacket.ms4525doData[i], sd_card_file);
    }
    // end writing data bytes to SD card
    
    *(DAQPacket*)pvParameters = daqPacket; // send data in packet back to main
  }

  // Unmount SD card
  fclose(sd_card_file);
  esp_vfs_fat_sdmmc_unmount();
  ESP_LOGI("daqLoop", "File closed and card unmounted");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoopDelay(void *pvParameters) {
  ESP_LOGW("killDaqLoopDelay", "stopping daq loop in 10000ms");
  vTaskDelay(10000 / portTICK_PERIOD_MS);
  aquireData = false;
  ESP_LOGW("killDaqLoopDelay", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoop(void *pvParameters) {
  aquireData = false;
  ESP_LOGW("killDaqLoop", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}
