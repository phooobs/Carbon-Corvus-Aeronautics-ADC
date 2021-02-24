#include <Wire.h> //I2C library 0x28H

void setup(void) {
   Serial.begin(115200);
   Wire.begin();
}

void loop() {
  Wire.requestFrom(0x28, 4, true); // request 4 bytes
  
  Serial.print(Wire.available());
  Serial.print(" ");
  if (Wire.available() == 4) {
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
    Serial.print(Wire.read(), HEX);
  }
  Wire.endTransmission(true);
  Serial.println();
  delay(2000);
}
