# Carbon-Corvus-Aeronautics-DAQ

**Fort lewis College Senior Seminar group 1: aero**

**CE Team:**
James Ferguson, Scott Orban

**GE Team:**
Austin McCaw, Elizabeth Blackwater, Orion Clay, Arielle Platero

## How to setup

For all of the following steps and installers, use the default settings. If you run into any issues please message James.

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install [ESP32 boards for arduino](https://www.youtube.com/watch?v=mBaS3YnqDaU&feature=emb_imp_woyt)
3. Install [Git](https://git-scm.com/download/win).
4. Run Windows PowerShell (It is preinstalled on Windows computers).
5. Type in the following commands.

    ``` bash
    git clone https://github.com/whoisjim/Carbon-Corvus-Aeronautics-DAQ
    ```

6. Setup is complete.

## How to program the ESP32 microcontroller

If you run into any issues please message James.

1. Plug in the ESP32.
2. Run Windows PowerShell (It is preinstalled on Windows computers).
3. Type in the following commands.

    ``` bash
    cd Carbon-Corvus-Aeronautics-DAQ/arduino/daq
    git pull
    ii .
    ```

4. Open the file `daq.ino` with the arduino IDE.
5. Click tools -> Board -> ESP32 Arduino -> ESP32 Dev Module
6. Click tools -> Port and select the port associated with the ESP32
7. Click the right pointing arrow button.
8. Verify that the program compiled and was flashed successfully.
9. Programming is complete, you may now unplug the ESP32.

## Todo

- [x] Multithreading
- [x] Gyroscope reading
- [x] Air speed sensor reading
- [x] SD card writing
- [x] SD card reading
- [ ] Online sensor fusion
- [x] Offline sensor fusion
- [ ] PWM input from radio module
- [ ] PWM output
- [ ] Active stability control
- [ ] Fail safe functionality
