/*
    This task is for system control functionality such as online sensor
    fusion, active stability, navigation, and fail safe functionality.
*/

#pragma once

#include "daqPacket.h"

void systemLoop (void * pvParameters) {
    struct DAQPacket daqPacket;
    for (;;) { //  loop  forever
        daqPacket = *(struct DAQPacket*)pvParameters; // update daq packet form main

        // test code
        vTaskDelay(1);
        printf("%i\n", daqPacket.testValue);

    }
}