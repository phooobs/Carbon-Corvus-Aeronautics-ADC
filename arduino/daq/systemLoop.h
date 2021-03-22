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

      //printf("%i ", (int16_t)(daqPacket.mpu9250Data[0][6] << 8) | daqPacket.mpu9250Data[0][7]);
      //printf("%i ", (int16_t)(daqPacket.mpu9250Data[0][8] << 8) | daqPacket.mpu9250Data[0][9]);
      //printf("%i\n", (int16_t)(daqPacket.mpu9250Data[0][10] << 8) | daqPacket.mpu9250Data[0][11]);
  }
}
