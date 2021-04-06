#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "daqPacket.h"
#include "daqLoop.h"
#include "systemLoop.h"

// uncomment to add delay to daq ack
#define DELAY_DAQ

DAQPacket daqPacket;

#define SW_PIN GPIO_NUM_35

void delayDaqLoopStart(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 60; i++) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(LED_PIN, LOW);
  }
  ESP_LOGI("app_main", "Launching DAQ task on core 1");
  xTaskCreatePinnedToCore(daqLoop, "DAQ Task", 10000, (void*)&daqPacket, 10, NULL, 1);
  ESP_LOGI("app_main", "Launching kill DAQ in 10 task on core 0");
  xTaskCreatePinnedToCore(killDaqLoopDelay, "DAQ Task", 10000, NULL, 2, NULL, 0);
  vTaskDelete(NULL); // kill this task
}

void setup() {
  ESP_LOGI("setup", "Carbon Corvus Aeronautics DAQ Startup");

  pinMode(SW_PIN, INPUT);
  
  // use uxTaskGetStackHighWaterMark() to figure out how big the stack sizes should actualy be
  ESP_LOGI("app_main", "Launching system task on core 0");
  xTaskCreatePinnedToCore(systemLoop, "System Task", 10000, (void*)&daqPacket, 1, NULL, 0);
  #ifdef DELAY_DAQ
    ESP_LOGI("app_main", "Launching delay DAQ task on core 1");
    xTaskCreatePinnedToCore(delayDaqLoopStart, "DAQ Task", 10000, (void*)&daqPacket, 10, NULL, 0);
  #endif
  #ifndef DELAY_DAQ
    ESP_LOGI("app_main", "Launching DAQ task on core 1");
    xTaskCreatePinnedToCore(daqLoop, "DAQ Task", 10000, (void*)&daqPacket, 10, NULL, 1);
    ESP_LOGI("app_main", "Launching kill DAQ in 10 task on core 0");
    xTaskCreatePinnedToCore(killDaqLoopDelay, "DAQ Task", 10000, NULL, 2, NULL, 0);
  #endif  
}

void loop() { // get switch input for starting daq
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  static bool last;
  bool now = digitalRead(SW_PIN);
  if (!now & last) { // falling edge turn off
    ESP_LOGI("app_main", "killing DAQ loop");
    xTaskCreatePinnedToCore(killDaqLoop, "DAQ Task", 10000, NULL, 2, NULL, 0);
  }
  last = now;
}
