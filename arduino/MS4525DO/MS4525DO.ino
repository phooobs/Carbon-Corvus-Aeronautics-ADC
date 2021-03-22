#include <Wire.h> //I2C library 0x28H

void setup(void) {
   Serial.begin(115200);
   Wire.begin();
}

void loop() {
  #define MS4525DO_BYTES 4
  #define MS4525DO_ADDRESS 0x28
  uint8_t ms4525doData[MS4525DO_BYTES] = {0, 0, 0, 0}; // data recived from the airspeed sentor
  uint8_t ms4525doStatus = 3; // 0 Ok, 1 Reserved, 2 Stale, 3 Error
  
  Wire.requestFrom(MS4525DO_ADDRESS, MS4525DO_BYTES, true); // request 4 bytes from airspeed sensor
  
  Serial.print(Wire.available());
  Serial.print(" : ");
  
  // read in requested bytes
  if (Wire.available() == MS4525DO_BYTES) {
    for (int i = 0; i < MS4525DO_BYTES; i++) {
      ms4525doData[i] = Wire.read();
      Serial.print(ms4525doData[i], HEX);
      Serial.print(" ");
    }
  }

  // get and remove statuc code from recived bytes
  ms4525doStatus = (ms4525doData[0] & 0xC0) >> 6;
  ms4525doData[0] = ms4525doData[0] & 0x3F;

  // format data
  uint16_t ms4525doDifferentialPressureRaw = ms4525doData[0] << 8 | ms4525doData[1];
  #define P_MIN -1.0f
	#define P_MAX 1.0f
	float ms4525doDifferentialPressure = -((ms4525doDifferentialPressureRaw - 0.1f * 16383) * (P_MAX - P_MIN) / (0.8f * 16383) + P_MIN);

  uint16_t ms4525doTemperatureRaw = 0xffe0 & (ms4525doData[2] << 8 | ms4525doData[3]) >> 5;
  float ms4525doTemperature = ((200.0f * ms4525doTemperatureRaw) / 2047) - 50;
  
  // print formatted data
  switch (ms4525doStatus) {
    case 0:
      Serial.print(": Ok ....... ");
      break;
    case 1:
      Serial.print(": Reserved . ");
      break;
    case 2:
      Serial.print(": Stale .... ");
      break;
    default:
      Serial.print(": Error .... ");
      break;
  }
  
  Serial.print(ms4525doDifferentialPressureRaw, HEX);
  //Serial.print(0.0211 * 100);
  Serial.print(" ");
  Serial.print(ms4525doTemperatureRaw, HEX);
  Serial.print(" : ");
  Serial.print(ms4525doDifferentialPressure, 4);
  Serial.print("PSI ");
  Serial.print(ms4525doTemperature, 4);
  Serial.print("C");
  Serial.println();

  Wire.endTransmission(true);
  delay(100);
}
