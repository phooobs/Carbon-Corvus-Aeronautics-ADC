/*
  This is the struct that the daq populates for the rest of the program
*/

#pragma once

struct DAQPacket {
  int testValue;
  uint8_t ms4525doData[4] = {0, 0, 0, 0}; // data recived from the airspeed sentor
};