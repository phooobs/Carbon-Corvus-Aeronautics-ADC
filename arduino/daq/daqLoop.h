/* 
    This task is for aquiring raw data and saving it to a sd card
    as well as making the data available to the rest of the program.
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <Wire.h>

#include "daqPacket.h"

// MPU9250 includes
#include <driver/spi_master.h>
#include "esp_system.h" //This inclusion configures the peripherals in the ESP system.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
////////////////////////////////////////////////////
#define evtGetIMU                  ( 1 << 1 ) // 10
///////////////////////////////////////////////////
EventGroupHandle_t eg;
///////////////////////////////////////////////////
#define TaskCore0 0
#define SerialDataBits 115200
#define CS01 27
#define CS02 5     
#define CS03 4   
#define spiCLK 18 // CLK module pin SCL
#define spiMOSI 23 // MOSI module pin SDA
#define spiMISO 19 // MISO module pin SDOAG tied to SDOM
#define TaskStack30K 30000
#define Priority4 4
#define MPU_int_Pin1 34
#define MPU_int_Pin2 17
#define MPU_int_Pin3 16
////////////////////////////////////////
spi_device_handle_t hAG1;
spi_device_handle_t hAG2;
spi_device_handle_t hAG3;
////////////////////////////////////////
const uint8_t SPI_READ = 0x80;
const uint8_t WHO_I_AMa = 0x73;
const uint8_t WHO_I_AMb = 0x71;
const uint8_t AK8963_IS = 0x48;
const uint8_t ACCELX_OUT = 0x3B;
const uint8_t ACCELY_OUT = 0x3D;
const uint8_t ACCELZ_OUT = 0x3F;
const uint8_t GYRO_OUTX = 0x43;
const uint8_t GYRO_OUTY = 0x45;
const uint8_t GYRO_OUTZ = 0x47;
const uint8_t EXT_SENS_DATA_00 = 0x49;
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t ACCEL_FS_SEL_16G = 0x18;
const uint8_t GYRO_CONFIG = 0x1B;
const uint8_t GYRO_FS_SEL_1000DPS = 0x10;
const uint8_t ACCEL_CONFIG2 = 0x1D;
const uint8_t ACCEL_DLPF_184 = 0x01;
const uint8_t CONFIG = 0x1A;
const uint8_t GYRO_DLPF_184 = 0x01;
const uint8_t SMPDIV = 0x19;
const uint8_t INT_PIN_CFG = 0x37;
const uint8_t INT_ENABLE = 0x38;
const uint8_t INT_DISABLE = 0x00;
const uint8_t INT_PULSE_50US = 0x00;
const uint8_t INT_RAW_RDY_EN = 0x01;
const uint8_t PWR_MGMNT_1 = 0x6B;
const uint8_t PWR_CYCLE = 0x20;
const uint8_t PWR_RESET = 0x80;
const uint8_t CLOCK_SEL_PLL = 0x01;
const uint8_t SEN_ENABLE = 0x00;
const uint8_t USER_CTRL = 0x6A;
const uint8_t I2C_MST_EN = 0x20;
const uint8_t I2C_MST_CLK = 0x0D;
const uint8_t I2C_MST_CTRL = 0x24;
const uint8_t I2C_SLV0_ADDR = 0x25;
const uint8_t I2C_SLV0_REG = 0x26;
const uint8_t I2C_SLV0_DO = 0x63;
const uint8_t I2C_SLV0_CTRL = 0x27;
const uint8_t I2C_SLV0_EN = 0x80;
const uint8_t I2C_READ_FLAG = 0x80;
const uint8_t WHO_AM_I = 0x75;
const uint8_t AK8963_I2C_ADDR = 0x0C;
const uint8_t AK8963_HXL = 0x03;
const uint8_t AK8963_CNTL1 = 0x0A;
const uint8_t AK8963_PWR_DOWN = 0x00;
const uint8_t AK8963_CNT_MEAS1 = 0x12;
const uint8_t AK8963_CNT_MEAS2 = 0x16;
const uint8_t AK8963_FUSE_ROM = 0x0F;
const uint8_t AK8963_CNTL2 = 0x0B;
const uint8_t AK8963_RESET = 0x01;
const uint8_t AK8963_ASA = 0x10;
const uint8_t AK8963_WHO_AM_I = 0x00;
////////////////////////////////////////////////
uint8_t txData[2] = { };
uint8_t rxData[21] = { };
////////////////////////////////////////////////

#define LED_PIN GPIO_NUM_33

// MPU9250 connections
// GPIO 27 -- CS1
// GPIO 5 -- CS2
// GPIO 4 -- CS3
// GPIO 34 -- INT1
// GPIO 17 -- INT2
// GPIO 16 -- INT3
// GPIO 18 -- CLK (all 3 chips)
// GPIO 23 -- MOSI (all 3 chips)
// GPIO 19 -- MISO (all 3 chips)

// SD card connections
// GPIO 15 -- CMD -- 10k pullup
// GPIO 14 -- CLK
// GPIO 2 -- GPIO 0 -- D0 -- 10k pullup
// D3 -- 10k pullup

// Airspeed sensor connections
// GPIO 22 -- SCL -- 10k pullup
// GPIO 21 -- SDA -- 10k pullup

#define MS4525DO_ADDRESS 0x28

// mpu9250 SPI functions

void triggerGet_IMU() {
  BaseType_t xHigherPriorityTaskWoken;
  xEventGroupSetBitsFromISR(eg, evtGetIMU, &xHigherPriorityTaskWoken);
}

void fReadMPU9250 ( uint8_t byteReadSize, uint8_t addressToRead, spi_device_handle_t hAG ) {
  esp_err_t intError;
  spi_transaction_t trans_desc;
  trans_desc = { };
  trans_desc.addr =  0;
  trans_desc.cmd = 0;
  trans_desc.flags = 0;
  trans_desc.length = (8 * 2) + (8 * byteReadSize) ; // total data bits
  trans_desc.tx_buffer = txData;
  trans_desc.rxlength = byteReadSize * 8 ; // Number of bits NOT number of bytes
  trans_desc.rx_buffer = rxData;
  txData[0] = addressToRead | SPI_READ;
  txData[1] = 0x0;
  intError = spi_device_transmit( hAG, &trans_desc);
  if ( intError != 0 )
  {
    Serial.print( " WHO I am MPU9250. Transmitting error = ");
    Serial.println ( intError );
  }
} // void fReadMPU9250 ( uint8_t byteReadSize, uint8_t addressToRead )

void fWriteSPIdata8bits( uint8_t address, uint8_t DataToSend, spi_device_handle_t hAG) {
  esp_err_t intError;
  spi_transaction_t trans_desc;
  trans_desc = { };
  trans_desc.addr = 0;
  trans_desc.cmd = 0;
  trans_desc.flags  = 0;
  trans_desc.length = 8 * 2; // total data bits
  trans_desc.tx_buffer = txData;
  txData[0] = address;
  txData[1] = DataToSend;
  intError = spi_device_transmit( hAG, &trans_desc);
  if ( intError != 0 )
  {
    Serial.print( " Transmitting error = ");
    Serial.println ( intError );
  }
}

void fReadAK8963( uint8_t subAddress, uint8_t count, spi_device_handle_t hAG ) {
  // set slave 0 to the AK8963 and set for read
  fWriteSPIdata8bits ( I2C_SLV0_ADDR, AK8963_I2C_ADDR | I2C_READ_FLAG, hAG );
  // set the register to the desired AK8963 sub address
  fWriteSPIdata8bits ( I2C_SLV0_REG, subAddress, hAG1 );
  // enable I2C and request the bytes
  fWriteSPIdata8bits ( I2C_SLV0_CTRL, I2C_SLV0_EN | count, hAG );
  vTaskDelay ( 1 );
  
}

void fWrite_AK8963 ( uint8_t subAddress, uint8_t dataAK8963, spi_device_handle_t hAG ) {
  fWriteSPIdata8bits ( I2C_SLV0_ADDR, AK8963_I2C_ADDR , hAG);
  fWriteSPIdata8bits ( I2C_SLV0_REG, subAddress, hAG );
  fWriteSPIdata8bits ( I2C_SLV0_DO, dataAK8963, hAG );
  fWriteSPIdata8bits ( I2C_SLV0_CTRL, I2C_SLV0_EN | (uint8_t)1, hAG) ;
  //
  vTaskDelay ( 1 );
}

void fwriteMUP9250register ( uint8_t addr, uint8_t sendData, spi_device_handle_t hAG ) {
  esp_err_t intError;
  spi_transaction_t trans_desc = {};
  trans_desc.addr =  addr;
  trans_desc.cmd = 0;
  trans_desc.flags = 0;
  trans_desc.length = 8 * 1 ; // total data bits
  trans_desc.tx_buffer = txData;
  txData[0] = sendData;
  Serial.print( " write mpu register " );
  Serial.print ( addr, HEX );
  Serial.print ( " data " );
  Serial.print ( sendData, HEX );
  Serial.print ( ". Transmitting error = ");
  intError = spi_device_transmit( hAG, &trans_desc);
  Serial.println ( intError );
}

// data aquizition loop
bool aquireData;
void daqLoop(void *pvParameters) {

  DAQPacket daqPacket;

  // mpu9250 accelerometer  SPI init
  eg = xEventGroupCreate();
  bool MPU9250_OK = false;
  bool AK8963_OK = false;
  esp_err_t intError;
  // data counts
  int16_t  _axcounts1, _aycounts1, _azcounts1;
  int16_t _gxcounts1, _gycounts1, _gzcounts1;
  int16_t _hxcounts1, _hycounts1, _hzcounts1;

  int16_t  _axcounts2, _aycounts2, _azcounts2;
  int16_t _gxcounts2, _gycounts2, _gzcounts2;
  int16_t _hxcounts2, _hycounts2, _hzcounts2;

  int16_t  _axcounts3, _aycounts3, _azcounts3;
  int16_t _gxcounts3, _gycounts3, _gzcounts3;
  int16_t _hxcounts3, _hycounts3, _hzcounts3;
  // data buffer
  float _accelScale;
  float _gyroScale;
  float _magScaleX, _magScaleY, _magScaleZ;
  ///////////////////////////////////////////////
  spi_bus_config_t bus_config = { };
  bus_config.sclk_io_num = spiCLK; // CLK
  bus_config.mosi_io_num = spiMOSI; // MOSI
  bus_config.miso_io_num = spiMISO; // MISO
  bus_config.quadwp_io_num = -1; // Not used
  bus_config.quadhd_io_num = -1; // Not used
  Serial.print(" Initializing bus error = ");
  intError = spi_bus_initialize(HSPI_HOST, &bus_config, 1) ;
  Serial.println ( intError );
  /////////////////////////////////////////////
  spi_device_interface_config_t dev1_config = { };  // initializes all field to 0
  dev1_config.address_bits     = 0;
  dev1_config.command_bits     = 0;
  dev1_config.dummy_bits       = 0;
  dev1_config.mode             = SPI_MODE3;
  dev1_config.duty_cycle_pos   = 0;
  dev1_config.cs_ena_posttrans = 0;
  dev1_config.cs_ena_pretrans  = 0;
  dev1_config.clock_speed_hz   = 1000000; // mpu 9250 spi read registers safe up to 1Mhz.
  dev1_config.spics_io_num     = CS01;
  dev1_config.flags            = 0;
  dev1_config.queue_size       = 1;
  dev1_config.pre_cb           = NULL;
  dev1_config.post_cb          = NULL;
  Serial.print (" Adding device bus error = ");
  intError = spi_bus_add_device(HSPI_HOST, &dev1_config, &hAG1);
  Serial.println ( intError );
  /////////////////////////////////////////////////
  spi_device_interface_config_t dev2_config = { };  // initializes all field to 0
  dev2_config.address_bits     = 0;
  dev2_config.command_bits     = 0;
  dev2_config.dummy_bits       = 0;
  dev2_config.mode             = SPI_MODE3;
  dev2_config.duty_cycle_pos   = 0;
  dev2_config.cs_ena_posttrans = 0;
  dev2_config.cs_ena_pretrans  = 0;
  dev2_config.clock_speed_hz   = 1000000; // mpu 9250 spi read registers safe up to 1Mhz.
  dev2_config.spics_io_num     = CS02;
  dev2_config.flags            = 0;
  dev2_config.queue_size       = 1;
  dev2_config.pre_cb           = NULL;
  dev2_config.post_cb          = NULL;
  Serial.print (" Adding device bus error = ");
  intError = spi_bus_add_device(HSPI_HOST, &dev2_config, &hAG2);
  Serial.println ( intError );
  /////////////////////////////////////////////////
  spi_device_interface_config_t dev3_config = { };  // initializes all field to 0
  dev3_config.address_bits     = 0;
  dev3_config.command_bits     = 0;
  dev3_config.dummy_bits       = 0;
  dev3_config.mode             = SPI_MODE3;
  dev3_config.duty_cycle_pos   = 0;
  dev3_config.cs_ena_posttrans = 0;
  dev3_config.cs_ena_pretrans  = 0;
  dev3_config.clock_speed_hz   = 1000000; // mpu 9250 spi read registers safe up to 1Mhz.
  dev3_config.spics_io_num     = CS03;
  dev3_config.flags            = 0;
  dev3_config.queue_size       = 1;
  dev3_config.pre_cb           = NULL;
  dev3_config.post_cb          = NULL;
  Serial.print (" Adding device bus error = ");
  intError = spi_bus_add_device(HSPI_HOST, &dev3_config, &hAG3);
  Serial.println ( intError );
  ////// spi_transaction_t trans_desc = { };
  fWrite_AK8963( AK8963_CNTL1, AK8963_PWR_DOWN, hAG1 );
  vTaskDelay ( 100 );
  // select clock source to gyro
  fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL, hAG1 );
  vTaskDelay(1);
  // Do who am I MPU9250
  fReadMPU9250 ( 2 , WHO_AM_I, hAG1 );
  if ( (WHO_I_AMa == rxData[1]) || (WHO_I_AMb == rxData[1]) )
  {
    MPU9250_OK = true;
  }
  else
  {
    Serial.print( " I am not MPU9250! I am: ");
    Serial.println ( rxData[1] );
  }
  if ( MPU9250_OK )
  {
    // enable I2C master mode
    fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN, hAG1 );
    vTaskDelay(1);
    // set the I2C bus speed to 400 kHz
    fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK, hAG1 );
    vTaskDelay(1);
    fWriteSPIdata8bits( SMPDIV, 0x04, hAG1 );
    vTaskDelay(1);
    fWriteSPIdata8bits( ACCEL_CONFIG2, ACCEL_DLPF_184, hAG1 );
    fWriteSPIdata8bits( CONFIG, GYRO_DLPF_184, hAG1 );
    vTaskDelay(1);
    // check AK8963_WHO_AM_I
    fReadAK8963( AK8963_WHO_AM_I, 1, hAG1);
    vTaskDelay(500); // giving the AK8963 lots of time to recover from reset
    fReadMPU9250( 2 , EXT_SENS_DATA_00, hAG1 );
    if ( AK8963_IS == rxData[1] )
    {
      AK8963_OK = true;
    }
    else
    {
      Serial.print ( " AK8963_OK data return = " );
      Serial.print ( rxData[0] );
      Serial.println ( rxData[1] );
    }
    if ( AK8963_OK )
    {
      // set AK8963 to FUSE ROM access
      fWrite_AK8963 ( AK8963_CNTL1, AK8963_FUSE_ROM, hAG1 );
      vTaskDelay ( 100 ); // delay for mode change
      //  // setting the accel range to 16G
      fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_16G, hAG1 );
      _accelScale = 16.0f / 32768.0f; // setting the accel scale to 16G
      // set gyro scale
      fWriteSPIdata8bits( GYRO_CONFIG, GYRO_FS_SEL_1000DPS, hAG1 );
      _gyroScale = 1000.0f / 32768.0f;
      // read the AK8963 ASA registers and compute magnetometer scale factors
      // set accel scale
      fReadAK8963(AK8963_ASA, 3, hAG1 );
      fReadMPU9250 ( 3 , EXT_SENS_DATA_00, hAG1 );
      // convert to mG multiply by 10
      _magScaleX = ((((float)rxData[0]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleY = ((((float)rxData[1]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleZ = ((((float)rxData[2]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      Serial.print( " _magScaleX " );
      Serial.print( _magScaleX );
      Serial.print( " , " );
      Serial.print( " _magScaleY " );
      Serial.print( _magScaleY );
      Serial.print( " , " );
      Serial.print( " _magScaleZ " );
      Serial.print( _magScaleZ );
      Serial.print( " , " );
      Serial.println();
      // set AK8963 to 16 bit resolution, 100 Hz update rate
      fWrite_AK8963( AK8963_CNTL1, AK8963_CNT_MEAS2, hAG1 );
      // delay for mode change
      vTaskDelay ( 100 );
      fReadAK8963(AK8963_HXL, 7, hAG1 );
      ///////////////////////////////////////////////
      /* setting the interrupt */
      // INT_PIN_CFG,INT_PULSE_50US setup interrupt, 50 us pulse
      fWriteSPIdata8bits( INT_PIN_CFG, INT_PULSE_50US, hAG1 );
      // INT_ENABLE,INT_RAW_RDY_EN set to data ready
      fWriteSPIdata8bits( INT_ENABLE, INT_RAW_RDY_EN, hAG1 );
      pinMode( MPU_int_Pin1, INPUT );
      attachInterrupt( MPU_int_Pin1, triggerGet_IMU, RISING );
    }
  }
  ////////////////////////////////////////////////////////////////
  // spi_transaction_t trans_desc = { };
  fWrite_AK8963( AK8963_CNTL1, AK8963_PWR_DOWN, hAG2 );
  vTaskDelay ( 100 );
  // select clock source to gyro
  fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL, hAG2 );
  vTaskDelay(1);
  // Do who am I MPU9250
  fReadMPU9250 ( 2 , WHO_AM_I, hAG2 );
  if ( (WHO_I_AMa == rxData[1]) || (WHO_I_AMb == rxData[1]) )
  {
    MPU9250_OK = true;
  }
  else
  {
    Serial.print( " I am not MPU9250! I am: ");
    Serial.println ( rxData[1] );
  }
  if ( MPU9250_OK )
  {
    // enable I2C master mode
    fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN, hAG2 );
    vTaskDelay(1);
    // set the I2C bus speed to 400 kHz
    fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK, hAG2 );
    vTaskDelay(1);
    fWriteSPIdata8bits( SMPDIV, 0x04, hAG2 );
    vTaskDelay(1);
    fWriteSPIdata8bits( ACCEL_CONFIG2, ACCEL_DLPF_184, hAG2 );
    fWriteSPIdata8bits( CONFIG, GYRO_DLPF_184, hAG2 );
    vTaskDelay(1);
    // check AK8963_WHO_AM_I
    fReadAK8963( AK8963_WHO_AM_I, 1, hAG2);
    vTaskDelay(500); // giving the AK8963 lots of time to recover from reset
    fReadMPU9250( 2 , EXT_SENS_DATA_00, hAG2 );
    if ( AK8963_IS == rxData[1] )
    {
      AK8963_OK = true;
    }
    else
    {
      Serial.print ( " AK8963_OK data return = " );
      Serial.print ( rxData[0] );
      Serial.println ( rxData[1] );
    }
    if ( AK8963_OK )
    {
      // set AK8963 to FUSE ROM access
      fWrite_AK8963 ( AK8963_CNTL1, AK8963_FUSE_ROM, hAG2 );
      vTaskDelay ( 100 ); // delay for mode change
      //// setting the accel range to 16G
      fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_16G, hAG2 );
      _accelScale = 16.0f / 32768.0f; // setting the accel scale to 16G
      // set gyro scale
      fWriteSPIdata8bits( GYRO_CONFIG, GYRO_FS_SEL_1000DPS, hAG2 );
      _gyroScale = 1000.0f / 32768.0f;
      // read the AK8963 ASA registers and compute magnetometer scale factors
      // set accel scale
      fReadAK8963(AK8963_ASA, 3, hAG2 );
      fReadMPU9250 ( 3 , EXT_SENS_DATA_00, hAG2 );
      // convert to mG multiply by 10
      _magScaleX = ((((float)rxData[0]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleY = ((((float)rxData[1]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleZ = ((((float)rxData[2]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      Serial.print( " _magScaleX " );
      Serial.print( _magScaleX );
      Serial.print( " , " );
      Serial.print( " _magScaleY " );
      Serial.print( _magScaleY );
      Serial.print( " , " );
      Serial.print( " _magScaleZ " );
      Serial.print( _magScaleZ );
      Serial.print( " , " );
      Serial.println();
      // set AK8963 to 16 bit resolution, 100 Hz update rate
      fWrite_AK8963( AK8963_CNTL1, AK8963_CNT_MEAS2, hAG2 );
      // delay for mode change
      vTaskDelay ( 100 );
      /* setting the interrupt */
      // INT_PIN_CFG,INT_PULSE_50US setup interrupt, 50 us pulse
      fWriteSPIdata8bits( INT_PIN_CFG, INT_PULSE_50US, hAG2 );
      // INT_ENABLE,INT_RAW_RDY_EN set to data ready
      fWriteSPIdata8bits( INT_ENABLE, INT_RAW_RDY_EN, hAG2 );
      pinMode( MPU_int_Pin1, INPUT );
      attachInterrupt( MPU_int_Pin1, triggerGet_IMU, RISING );
    }
  }
  ////////////////////////////////////////////////////////////////
   ////// spi_transaction_t trans_desc = { };
  fWrite_AK8963( AK8963_CNTL1, AK8963_PWR_DOWN, hAG3 );
  vTaskDelay ( 100 );
  // select clock source to gyro
  fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL, hAG3 );
  vTaskDelay(1);
  // Do who am I MPU9250
  fReadMPU9250 ( 2 , WHO_AM_I, hAG3 );
  if ( (WHO_I_AMa == rxData[1]) || (WHO_I_AMb == rxData[1]) )
  {
    MPU9250_OK = true;
  }
  else
  {
    Serial.print( " I am not MPU9250! I am: ");
    Serial.println ( rxData[1] );
  }
  if ( MPU9250_OK )
  {
    // enable I2C master mode
    fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN, hAG3 );
    vTaskDelay(1);
    // set the I2C bus speed to 400 kHz
    fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK, hAG3 );
    vTaskDelay(1);
    fWriteSPIdata8bits( SMPDIV, 0x04, hAG3 );
    vTaskDelay(1);
    fWriteSPIdata8bits( ACCEL_CONFIG2, ACCEL_DLPF_184, hAG3 );
    fWriteSPIdata8bits( CONFIG, GYRO_DLPF_184, hAG3 );
    vTaskDelay(1);
    // check AK8963_WHO_AM_I
    fReadAK8963( AK8963_WHO_AM_I, 1, hAG3);
    vTaskDelay(500); // giving the AK8963 lots of time to recover from reset
    fReadMPU9250( 2 , EXT_SENS_DATA_00, hAG3 );
    if ( AK8963_IS == rxData[1] )
    {
      AK8963_OK = true;
    }
    else
    {
      Serial.print ( " AK8963_OK data return = " );
      Serial.print ( rxData[0] );
      Serial.println ( rxData[1] );
    }
    if ( AK8963_OK )
    {
      // set AK8963 to FUSE ROM access
      fWrite_AK8963 ( AK8963_CNTL1, AK8963_FUSE_ROM, hAG3 );
      vTaskDelay ( 100 ); // delay for mode change
      //  // setting the accel range to 16G
      fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_16G, hAG3 );
      _accelScale = 16.0f / 32768.0f; // setting the accel scale to 16G
      // set gyro scale
      fWriteSPIdata8bits( GYRO_CONFIG, GYRO_FS_SEL_1000DPS, hAG3 );
      _gyroScale = 1000.0f / 32768.0f;
      // read the AK8963 ASA registers and compute magnetometer scale factors
      // set accel scale
      fReadAK8963(AK8963_ASA, 3, hAG3 );
      fReadMPU9250 ( 3 , EXT_SENS_DATA_00, hAG3 );
      // convert to mG multiply by 10
      _magScaleX = ((((float)rxData[0]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleY = ((((float)rxData[1]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      _magScaleZ = ((((float)rxData[2]) - 128.0f) / (256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
      Serial.print( " _magScaleX " );
      Serial.print( _magScaleX );
      Serial.print( " , " );
      Serial.print( " _magScaleY " );
      Serial.print( _magScaleY );
      Serial.print( " , " );
      Serial.print( " _magScaleZ " );
      Serial.print( _magScaleZ );
      Serial.print( " , " );
      Serial.println();
      // set AK8963 to 16 bit resolution, 100 Hz update rate
      fWrite_AK8963( AK8963_CNTL1, AK8963_CNT_MEAS2, hAG3 );
      // delay for mode change
      vTaskDelay ( 100 );
      fReadAK8963(AK8963_HXL, 7, hAG3 );
      ///////////////////////////////////////////////
      /* setting the interrupt */
      // INT_PIN_CFG,INT_PULSE_50US setup interrupt, 50 us pulse
      fWriteSPIdata8bits( INT_PIN_CFG, INT_PULSE_50US, hAG3 );
      // INT_ENABLE,INT_RAW_RDY_EN set to data ready
      fWriteSPIdata8bits( INT_ENABLE, INT_RAW_RDY_EN, hAG3 );
      pinMode( MPU_int_Pin3, INPUT );
      attachInterrupt( MPU_int_Pin3, triggerGet_IMU, RISING );
    }
  }

  // start I2C for airspeed sensor
  Wire.begin();

  // 1-line SD card init
  ESP_LOGI("daqLoop", "Attempting to mount SDMMC peripheral at /sd");
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 1;

  // Set internal GPIO pullups for SD card pins
  gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY); // CMD
  gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);  // D0

  // Options for mounting the filesystem.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  // Try to mount SD card at /sd
  sdmmc_card_t *card;
  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sd", &host, &slot_config, &mount_config, &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE("daqLoop", "Failed to mount filesystem. "
                          "If you want the card to be formatted, set format_if_mount_failed = true.");
    } else {
      ESP_LOGE("daqLoop", "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    vTaskDelete(NULL); // kill this task
  }

  // find file index and get file path
  int fileIndex = 0;
  char filePath[11];
  while (true) {
    sprintf(filePath, "/sd/%i.raw", fileIndex);
    FILE *sd_card_file = fopen(filePath, "r");
    if (sd_card_file == NULL) { // file at index does not exist, valid index, break
      break;
    } else {
      fileIndex++;
    }
    fclose(sd_card_file);
  }

  // Create and open file
  // note there is a file name length limit
  FILE *sd_card_file = fopen(filePath, "w");
  if (sd_card_file == NULL) {
    ESP_LOGE("daqLoop", "Failed to open file for writing");
    vTaskDelete(NULL); // kill this task
  }

  ESP_LOGI("daqLoop", "Card mounted and file openend");
  
  daqPacket.testValue = 0;
  aquireData = true;

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  while (aquireData) { // loop untill told not to

    // test code
    daqPacket.testValue++;

    // mpu9250 request data from each sensor
    xEventGroupWaitBits (eg, evtGetIMU, pdTRUE, pdTRUE, portMAX_DELAY);
    if ( MPU9250_OK && AK8963_OK ) {

      // put data into daqPacket
      fReadMPU9250 ( 6 , 0X3A, hAG1 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[0][i] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , GYRO_OUTX, hAG1 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[0][i + 6] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , EXT_SENS_DATA_00, hAG1 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[0][i + 12] = rxData[2 + i];
      }
      
      fReadMPU9250 ( 6 , 0X3A, hAG2 );
      // put accelerometer data into daqPacket
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[1][i] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , GYRO_OUTX, hAG2 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[1][i + 6] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , EXT_SENS_DATA_00, hAG2 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[1][i + 12] = rxData[2 + i];
      }

      fReadMPU9250 ( 6 , 0X3A, hAG3 );
      // put accelerometer data into daqPacket
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[2][i] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , GYRO_OUTX, hAG3 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[2][i + 6] = rxData[2 + i];
      }
      fReadMPU9250 ( 6 , EXT_SENS_DATA_00, hAG3 );
      for (int i = 0; i < 6; i++) {
        daqPacket.mpu9250Data[2][i + 12] = rxData[2 + i];
      }

    } else {
      ESP_LOGE("MPU9250 WHO AM I FAILED!!");
    }

    // request and read 4 bytes from airspeed sensor
    #define MS4525DO_BYTES 4
    Wire.requestFrom(MS4525DO_ADDRESS, MS4525DO_BYTES, true);
    if (Wire.available() == MS4525DO_BYTES) {
      for (int i = 0; i < MS4525DO_BYTES; i++) {
        daqPacket.ms4525doData[i] = Wire.read();
      }
    } else { // did not recive the correct number of bytes
      int bytes = Wire.available();
      daqPacket.ms4525doData[0] = 0xC0; 
      for (int i = 0; i < bytes; i++) { // flush wire
        Wire.read();
      }
    }

    // write bytes to SD card
    putc(0xff, sd_card_file); // delimiter byte 0xff
    uint32_t timeBytes = esp_timer_get_time();
    putc((uint8_t)(timeBytes >> 24), sd_card_file); // timeing bytes in microsecconds
    putc((uint8_t)(timeBytes >> 16), sd_card_file);
    putc((uint8_t)(timeBytes >> 8), sd_card_file);
    putc((uint8_t)(timeBytes >> 0), sd_card_file);
    // write ms4525do data
    for (int i = 0; i < MS4525DO_BYTES; i++) {
      putc(daqPacket.ms4525doData[i], sd_card_file);
    }
    // write mpu9250 data
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 18; j++) {
        putc(daqPacket.mpu9250Data[i][j], sd_card_file);
      }
    }
    // end writing data bytes to SD card
    
    *(DAQPacket*)pvParameters = daqPacket; // send data in packet back to main
  }

  digitalWrite(LED_PIN, LOW);
  
  // Unmount SD card
  fclose(sd_card_file);
  esp_vfs_fat_sdmmc_unmount();
  ESP_LOGI("daqLoop", "File closed and card unmounted");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoopDelay(void *pvParameters) {
  ESP_LOGW("killDaqLoopDelay", "stopping daq loop in 600000ms"); // 10min
  vTaskDelay(600000 / portTICK_PERIOD_MS);
  aquireData = false;
  ESP_LOGW("killDaqLoopDelay", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}

void killDaqLoop(void *pvParameters) {
  aquireData = false;
  ESP_LOGW("killDaqLoop", "daq loop stopped");
  vTaskDelete(NULL); // kill this task
}
