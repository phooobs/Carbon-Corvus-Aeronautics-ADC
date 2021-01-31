/* 
    This task is for aquiring raw data and saving it to a sd card
    as well as making the data available to the rest of the program.
*/

#pragma once

#include "daqPacket.h"

void daqLoop (void * pvParameters) {
    struct DAQPacket daqPacket;
    daqPacket.testValue = 0;
    for (;;) { // loop  forever

        // test code
        daqPacket.testValue++;
        vTaskDelay(10);

        *(struct DAQPacket*)pvParameters = daqPacket; // send data in packet back to main
    }
}