# Carbon-Corvus-Aeronautics-DAQ

**Fort lewis College Senior Seminar group 1: aero**

**CE Team:**
James Ferguson, Scott Orban

**GE Team:**
Austin McCaw, Elizabeth Blackwater, Orion Clay, Arielle Platero

## How to setup for windows

For all of the following steps and installers, use the default settings. If you run into any issues please message James.

1. Install [VS code](https://code.visualstudio.com/Download).
2. Install [Python 3](https://www.python.org/downloads/).
3. Install [Git](https://git-scm.com/download/win).
4. Run Windows PowerShell (It is preinstalled on Windows computers).
5. Type in the following commands.

    ``` bash
    git clone https://github.com/phooobs/Carbon-Corvus-Aeronautics-DAQ
    cd Carbon-Corvus-Aeronautics-DAQ/daq
    code .
    ```

6. VS code should have opened install the following extentions from within.
    1. Install [Espressif IDF](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension).
    2. Install [Live Share](https://marketplace.visualstudio.com/items?itemName=MS-vsliveshare.vsliveshare) (optional, for remote assistance).
7. Press F1 and then type in `ESP-IDF: Configure ESP-IDF extension` followed by pressing enter.
8. Follow on-screen instructions.
9. Done. You can close VS code and all other windows.

## How to program the ESP32 microcontroller

If you run into any issues please message James.

1. Plug in the ESP32.
2. Run Windows PowerShell (It is preinstalled on Windows computers).
3. Type in the following commands.

    ``` bash
    cd Carbon-Corvus-Aeronautics-DAQ/daq
    git pull
    code .
    ```

4. Click the plug icon at the bottom of the window and select the COM port that you plugged the ESP32 into.
5. Click the flame at the bottom of the window.
6. A terminal will open and begin compiling and flashing the ESP32, wait till the process finishes.
7. Done. You may unplug the ESP32 now.

## Todo

- [x] Multithreading
- [ ] Gyroscope reading `Scott`
- [ ] Air speed sensor reading
- [ ] SD card writing `James`
- [ ] SD card reading
- [ ] Online sensor fusion
- [ ] Offline sensor fusion
- [ ] PWM input from radio module
- [ ] PWM output
- [ ] Active stability control
- [ ] Fail safe functionality
