/*
  This Struct is used to share sensor data with the rest of the system
*/

#pragma once

struct DAQPacket {
  int testValue;
  uint8_t ms4525doData[4] = {0, 0, 0, 0}; // data recived from the airspeed sentor
  uint8_t mpu6050Data[3][12]; // data recived from the accelerometer
};
