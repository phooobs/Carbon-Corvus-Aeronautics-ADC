/*
  This task is for system control functionality such as online sensor
  fusion, active stability, navigation, and fail safe functionality.
*/

#pragma once

#include "daqPacket.h"

void systemLoop(void *pvParameters){
  DAQPacket daqPacket;
  while (true) { //  loop  forever
    daqPacket = *(DAQPacket *)pvParameters; // update daq packet form main

    // test code
    vTaskDelay(100);
    ESP_LOGI("systemLoop", "%i", daqPacket.testValue);
  }
}
