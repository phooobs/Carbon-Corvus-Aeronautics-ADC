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

#define MS4525DO_BYTES 4
#define MS4525DO_ADDRESS 0x28
#define MPU6050_ADDRESS 0x68

// chip select is active low
#define MPU6050_0CS 27
#define MPU6050_1CS 5 
#define MPU6050_2CS 4

#define LED_PIN GPIO_NUM_33

// MPU9250 connections
// GPIO 27 -- CS0
// GPIO 5 -- CS1
// GPIO 4 -- CS2
// GPIO 18 -- CLK (all 3 chips)
// GPIO 23 -- MOSI (all 3 chips)
// GPIO 19 -- MISO (all 3 chips)

// SD card connections
// GPIO 15 -- CMD -- 10k pullup
// GPIO 14 -- CLK
// GPIO 2 -- GPIO 0 -- D0 -- 10k pullup
// D3 -- 10k pullup

// Airspeed sensor connections
// GPIO 22 -- SCL -- 10k pullup
// GPIO 21 -- SDA -- 10k pullup

// data aquisition loop
bool aquireData;
void daqLoop(void *pvParameters) {

  DAQPacket daqPacket;

  // start I2C for airspeed sensor
  Wire.begin();
  for (int i = 0; i < 3; i++) {
    if (i == 0) {
      digitalWrite(MPU6050_0CS, LOW);
      digitalWrite(MPU6050_1CS, HIGH);
      digitalWrite(MPU6050_2CS, HIGH);
    } else if (i == 2) {
      digitalWrite(MPU6050_0CS, HIGH);
      digitalWrite(MPU6050_1CS, LOW);
      digitalWrite(MPU6050_2CS, HIGH);
    } else { // i == 3
      digitalWrite(MPU6050_0CS, HIGH);
      digitalWrite(MPU6050_1CS, HIGH);
      digitalWrite(MPU6050_2CS, LOW);
    }
    
    // configure mpu6050
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(0x1B); // select GYRO_CONFIG
    Wire.write(0x08); // write 00001000, no self test, +/- 500 degrees/s
    Wire.endTransmission();

    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(0x6B); // select PWR_MGMT_1
    Wire.write(0x00); // write 000000000, turn off sleep mode use 8MHz oscilator
    Wire.endTransmission();

    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(0x1C); // select ACCEL_CONFIG
    Wire.write(0x08); // write 00001000, no self test, +/- 4g
    Wire.endTransmission();
  }

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

  // find file index and get file path
  int fileIndex = 0;
  char filePath[11];
  while (true) {
    sprintf(filePath, "/sd/%i.raw", fileIndex);
    FILE *sd_card_file = fopen(filePath, "r");
    if (sd_card_file == NULL) { // file at index does not exist, valid index, break
      break;
    } else {
      fileIndex++;
    }
    fclose(sd_card_file);
  }

  // Create and open file
  // note there is a file name length limit
  FILE *sd_card_file = fopen(filePath, "w");
  if (sd_card_file == NULL) {
    ESP_LOGE("daqLoop", "Failed to open file for writing");
    vTaskDelete(NULL); // kill this task
  }

  ESP_LOGI("daqLoop", "Card mounted and file openend");
  
  daqPacket.testValue = 0;
  aquireData = true;

  pinMode(LED_PIN, OUTPUT);
  pinMode(MPU6050_0CS, OUTPUT);
  pinMode(MPU6050_1CS, OUTPUT);
  pinMode(MPU6050_2CS, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  while (aquireData) { // loop until told not to

    // test code
    daqPacket.testValue++;

    // get mpu6050 data from all three
    for (int i = 0; i < 3; i++) {
      if (i == 0) {
        digitalWrite(MPU6050_0CS, LOW);
        digitalWrite(MPU6050_1CS, HIGH);
        digitalWrite(MPU6050_2CS, HIGH);
      } else if (i == 2) {
        digitalWrite(MPU6050_0CS, HIGH);
        digitalWrite(MPU6050_1CS, LOW);
        digitalWrite(MPU6050_2CS, HIGH);
      } else { // i == 3
        digitalWrite(MPU6050_0CS, HIGH);
        digitalWrite(MPU6050_1CS, HIGH);
        digitalWrite(MPU6050_2CS, LOW);
      }

      // get accelerometer
      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(0x3B); // start at ACCEL_XOUT_H
      Wire.endTransmission(false);
      // request registers ACCEL_XOUT_H through ACCEL_ZOUT_L
      Wire.requestFrom(MPU6050_ADDRESS, 6, true); // request 6 bytes
      if (Wire.available() == 6) {
        for (int j = 0; j < 6; j++) {
          daqPacket.mpu6050Data[i][j] = Wire.read();
        }
      }
      Wire.endTransmission(true);

      // get gyroscope
      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(0x43); // start at GYRO_XOUT_H
      Wire.endTransmission(false);
      // request registers GYRO_XOUT_H through GYRO_ZOUT_L
      Wire.requestFrom(MPU6050_ADDRESS, 6, true); // request 6 bytes
      if (Wire.available() == 6) {
        for (int j = 6; j < 12; j++) {
          daqPacket.mpu6050Data[i][j] = Wire.read();
        }
      }
      Wire.endTransmission(true);
    }

    // request and read 4 bytes from airspeed sensor
    #define MS4525DO_BYTES 4
    Wire.requestFrom(MS4525DO_ADDRESS, MS4525DO_BYTES, true);
    if (Wire.available() == MS4525DO_BYTES) {
      for (int i = 0; i < MS4525DO_BYTES; i++) {
        daqPacket.ms4525doData[i] = Wire.read();
      }
    } else { // did not recive the correct number of bytes
      int bytes = Wire.available();
      daqPacket.ms4525doData[0] = 0xC0; 
      for (int i = 0; i < bytes; i++) { // flush wire
        Wire.read();
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
    // write mpu9250 data
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 12; j++) {
        putc(daqPacket.mpu6050Data[i][j], sd_card_file);
      }
    }
    // end writing data bytes to SD card
    
    *(DAQPacket*)pvParameters = daqPacket; // send data in packet back to main
  }

  digitalWrite(LED_PIN, LOW);
  
  // Unmount SD card
  fclose(sd_card_file);
  esp_vfs_fat_sdmmc_unmount();
  ESP_LOGI("daqLoop", "File closed and card unmounted");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoopDelay(void *pvParameters) {
  ESP_LOGW("killDaqLoopDelay", "stopping daq loop in 600000ms"); // 10min
  vTaskDelay(600000 / portTICK_PERIOD_MS);
  aquireData = false;
  ESP_LOGW("killDaqLoopDelay", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoop(void *pvParameters) {
  aquireData = false;
  ESP_LOGW("killDaqLoop", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}
