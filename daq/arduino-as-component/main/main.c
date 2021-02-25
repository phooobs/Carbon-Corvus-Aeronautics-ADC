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
#define TaskCore1 1
#define TaskCore0 0
#define SerialDataBits 115200
#define csPinAG 5
#define csPinM 32
#define spiCLK 18 // CLK module pin SCL
#define spiMOSI 23 // MOSI module pin SDA
#define spiMISO 19 // MISO module pin SDOAG tied to SDOM
#define TaskStack30K 30000
#define Priority4 4
#define MPU_int_Pin 34
////////////////////////////////////////
spi_device_handle_t hAG;
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
// transformation matrix
/* transform the accel and gyro axes to match the magnetometer axes */
// constants
const uint8_t EXT_SENS_DATA_00 = 0x49;
const float G = 9.807f;
const float _d2r = 3.14159265359f / 180.0f;
const int16_t tX[3] = {0,  1,  0};
const int16_t tY[3] = {1,  0,  0};
const int16_t tZ[3] = {0,  0, -1};
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t ACCEL_FS_SEL_2G = 0x00;
const uint8_t ACCEL_FS_SEL_4G = 0x08;
const uint8_t ACCEL_FS_SEL_8G = 0x10;
const uint8_t ACCEL_FS_SEL_16G = 0x18;
const uint8_t GYRO_CONFIG = 0x1B;
const uint8_t GYRO_FS_SEL_250DPS = 0x00;
const uint8_t GYRO_FS_SEL_500DPS = 0x08;
const uint8_t GYRO_FS_SEL_1000DPS = 0x10;
const uint8_t GYRO_FS_SEL_2000DPS = 0x18;
const uint8_t ACCEL_CONFIG2 = 0x1D;
const uint8_t ACCEL_DLPF_184 = 0x01;
const uint8_t ACCEL_DLPF_92 = 0x02;
const uint8_t ACCEL_DLPF_41 = 0x03;
const uint8_t ACCEL_DLPF_20 = 0x04;
const uint8_t ACCEL_DLPF_10 = 0x05;
const uint8_t ACCEL_DLPF_5 = 0x06;
const uint8_t CONFIG = 0x1A;
const uint8_t GYRO_DLPF_184 = 0x01;
const uint8_t GYRO_DLPF_92 = 0x02;
const uint8_t GYRO_DLPF_41 = 0x03;
const uint8_t GYRO_DLPF_20 = 0x04;
const uint8_t GYRO_DLPF_10 = 0x05;
const uint8_t GYRO_DLPF_5 = 0x06;
const uint8_t SMPDIV = 0x19;
const uint8_t INT_PIN_CFG = 0x37;
const uint8_t INT_ENABLE = 0x38;
const uint8_t INT_DISABLE = 0x00;
const uint8_t INT_PULSE_50US = 0x00;
const uint8_t INT_WOM_EN = 0x40;
const uint8_t INT_RAW_RDY_EN = 0x01;
const uint8_t PWR_MGMNT_1 = 0x6B;
const uint8_t PWR_CYCLE = 0x20;
const uint8_t PWR_RESET = 0x80;
const uint8_t CLOCK_SEL_PLL = 0x01;
const uint8_t PWR_MGMNT_2 = 0x6C;
const uint8_t SEN_ENABLE = 0x00;
const uint8_t DIS_GYRO = 0x07;
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
const uint8_t MOT_DETECT_CTRL = 0x69;
const uint8_t ACCEL_INTEL_EN = 0x80;
const uint8_t ACCEL_INTEL_MODE = 0x40;
const uint8_t LP_ACCEL_ODR = 0x1E;
const uint8_t WOM_THR = 0x1F;
const uint8_t WHO_AM_I = 0x75;
const uint8_t FIFO_EN = 0x23;
const uint8_t FIFO_TEMP = 0x80;
const uint8_t FIFO_GYRO = 0x70;
const uint8_t FIFO_ACCEL = 0x08;
const uint8_t FIFO_MAG = 0x01;
const uint8_t FIFO_COUNT = 0x72;
const uint8_t FIFO_READ = 0x74;
// AK8963 registers
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
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};           // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};              // vector to hold integral error for Mahony method
float deltat = 0.0f;                             // integration interval for both filter schemes
#define Kp 7.50f
// #define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 1.7f
uint8_t txData[2] = { };
uint8_t rxData[21] = { };
////////////////////////////////////////////////
void triggerGet_IMU()
{
  BaseType_t xHigherPriorityTaskWoken;
  xEventGroupSetBitsFromISR(eg, evtGetIMU, &xHigherPriorityTaskWoken);
}
///////////////////////////////////////////////////
void loop() {}
////
void fGetIMU( void *pvParameters )
{
  bool MPU9250_OK = false;
  bool AK8963_OK = false;
  esp_err_t intError;
  // data counts
  int16_t  _axcounts, _aycounts, _azcounts;
  int16_t _gxcounts, _gycounts, _gzcounts;
  int16_t _hxcounts, _hycounts, _hzcounts;
  int16_t _tcounts;
  // data buffer
  float _ax, _ay, _az;
  float _gx, _gy, _gz;
  float _hx, _hy, _hz;
  float _t;
  float _accelScale;
  float _gyroScale;
  float _magScaleX, _magScaleY, _magScaleZ;
  float _axb, _ayb, _azb;
  float _gxb, _gyb, _gzb;
  float _axs = 1.0f;
  float _ays = 1.0f;
  float _azs = 1.0f;
  float _hxb, _hyb, _hzb;
  float _hxs = 1.0f;
  float _hys = 1.0f;
  float _hzs = 1.0f;
  ////
  spi_bus_config_t bus_config = { };
  bus_config.sclk_io_num = spiCLK; // CLK
  bus_config.mosi_io_num = spiMOSI; // MOSI
  bus_config.miso_io_num = spiMISO; // MISO
  bus_config.quadwp_io_num = -1; // Not used
  bus_config.quadhd_io_num = -1; // Not used
//   Serial.print(" Initializing bus error = ");
  intError = spi_bus_initialize(HSPI_HOST, &bus_config, 1) ;
//   Serial.println ( intError );
  //
  spi_device_interface_config_t dev_config = { };  // initializes all field to 0
  dev_config.address_bits     = 0;
  dev_config.command_bits     = 0;
  dev_config.dummy_bits       = 0;
//   dev_config.mode             = SPI_MODE3 ;
  dev_config.mode             = 3 ;
  dev_config.duty_cycle_pos   = 0;
  dev_config.cs_ena_posttrans = 0;
  dev_config.cs_ena_pretrans  = 0;
  dev_config.clock_speed_hz   = 1000000; // mpu 9250 spi read registers safe up to 1Mhz.
  dev_config.spics_io_num     = csPinAG;
  dev_config.flags            = 0;
  dev_config.queue_size       = 1;
  dev_config.pre_cb           = NULL;
  dev_config.post_cb          = NULL;
//   Serial.print (" Adding device bus error = ");
  intError = spi_bus_add_device(HSPI_HOST, &dev_config, &hAG);
//   Serial.println ( intError );
  ////
  ////
  // spi_transaction_t trans_desc = { };
  fWrite_AK8963( AK8963_CNTL1, AK8963_PWR_DOWN );
  vTaskDelay ( 100 );
  // fWriteSPIdata8bits( PWR_MGMNT_1, PWR_RESET );
  // wait for MPU-9250 to come back up
  // vTaskDelay(100);
  // select clock source to gyro
  // PWR_MGMNT_1,CLOCK_SEL_PLL
  fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL );
  vTaskDelay(1);
  // Do who am I MPU9250
  fReadMPU9250 ( 2 , WHO_AM_I );
  if ( (WHO_I_AMa == rxData[1]) || (WHO_I_AMb == rxData[1]) )
  {
    MPU9250_OK = true;
  }
  else
  {
    // Serial.print( " I am not MPU9250! I am: ");
    // Serial.println ( rxData[1] );
  }
  if ( MPU9250_OK )
  {
    // enable I2C master mode
    // USER_CTRL,I2C_MST_EN
    fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN );
    vTaskDelay(1);
    // set the I2C bus speed to 400 kHz
    // I2C_MST_CTRL,I2C_MST_CLK
    fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK );
    vTaskDelay(1);
    fWriteSPIdata8bits( SMPDIV, 0x04 );
    vTaskDelay(1);
    // DLPF ACCEL_CONFIG2,ACCEL_DLPF_184
    fWriteSPIdata8bits( ACCEL_CONFIG2, ACCEL_DLPF_184 );
    // CONFIG,GYRO_DLPF_184
    fWriteSPIdata8bits( CONFIG, GYRO_DLPF_184 );
    vTaskDelay(1);
    // check AK8963_WHO_AM_I
    fReadAK8963( AK8963_WHO_AM_I, 1 );
    vTaskDelay(500); // giving the AK8963 lots of time to recover from reset
    fReadMPU9250( 2 , EXT_SENS_DATA_00 );
    if ( AK8963_IS == rxData[1] )
    {
      AK8963_OK = true;
    }
    else
    {
    //   Serial.print ( " AK8963_OK data return = " );
    //   Serial.print ( rxData[0] );
    //   Serial.println ( rxData[1] );
    }
    if ( AK8963_OK )
    {
      // set AK8963 to FUSE ROM access
      // AK8963_CNTL1,AK8963_FUSE_ROM
      fWrite_AK8963 ( AK8963_CNTL1, AK8963_FUSE_ROM );
      vTaskDelay ( 100 ); // delay for mode change
      //  // setting the accel range to 16G
      fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_16G );
      _accelScale = 16.0f / 32768.0f; // setting the accel scale to 16G
      //   ACCEL_CONFIG,ACCEL_FS_SEL_8G
      // fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_8G );
      // _accelScale = 8.0f/32768.0f; // setting the accel scale to 8G
      //    ACCEL_CONFIG,ACCEL_FS_SEL_4G
      // fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_4G );
      //  _accelScale = 4.0f/32768.0f; // setting the accel scale to 4G
      // ACCEL_CONFIG,ACCEL_FS_SEL_2G
      // fWriteSPIdata8bits( ACCEL_CONFIG, ACCEL_FS_SEL_2G );
      // _accelScale = 2.0f/32768.0f;
      // // _accelScale = G * 2.0f / 32767.5f; // setting the accel scale to 2G
      // set gyro scale
      // GYRO_CONFIG,GYRO_FS_SEL_1000DPS
      fWriteSPIdata8bits( GYRO_CONFIG, GYRO_FS_SEL_1000DPS );
      // 250.0f/32768.0f;
      // 500.0f/32768.0f;
      // 2000.0f/32768.0f;
      _gyroScale = 1000.0f / 32768.0f;
      // read the AK8963 ASA registers and compute magnetometer scale factors
      //   set accel scale
      fReadAK8963(AK8963_ASA, 3 );
      fReadMPU9250 ( 3 , EXT_SENS_DATA_00 );
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
      // AK8963_CNTL1,AK8963_CNT_MEAS2
      fWrite_AK8963( AK8963_CNTL1, AK8963_CNT_MEAS2 );
      // delay for mode change
      vTaskDelay ( 100 );
      // AK8963_HXL,7 ;
      fReadAK8963(AK8963_HXL, 7 );
      ////
      /* setting the interrupt */
      // INT_PIN_CFG,INT_PULSE_50US setup interrupt, 50 us pulse
      fWriteSPIdata8bits( INT_PIN_CFG, INT_PULSE_50US );
      // INT_ENABLE,INT_RAW_RDY_EN set to data ready
      fWriteSPIdata8bits( INT_ENABLE, INT_RAW_RDY_EN );
      pinMode( MPU_int_Pin, INPUT );
      attachInterrupt( MPU_int_Pin, triggerGet_IMU, RISING );
    }
  }
  ////////////////////////////////////////////////////////////////
  TickType_t TimePast = xTaskGetTickCount();
  TickType_t TimeNow = xTaskGetTickCount();

  TickType_t xLastWakeTime;
  // const TickType_t xFrequency = pdMS_TO_TICKS( 100 );
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  ////
  while (1)
  {
    xEventGroupWaitBits (eg, evtGetIMU, pdTRUE, pdTRUE, portMAX_DELAY);
     if ( MPU9250_OK && AK8963_OK )
    {
      TimeNow = xTaskGetTickCount();
      deltat = ( (float)TimeNow - (float)TimePast) / 1000.0f;
      ////
      // Serial.println ( " doing a loop MPU OK" );
      fReadMPU9250 ( 8 , 0X3A );
      _axcounts = (int16_t)(((int16_t)rxData[2] << 8) | rxData[3]) ;
      _aycounts = (int16_t)(((int16_t)rxData[4] << 8) | rxData[5]) ;
      _azcounts = (int16_t)(((int16_t)rxData[6] << 8) | rxData[7]) ;
       fReadMPU9250 ( 6 , GYRO_OUTX );
      _gxcounts = (int16_t)(((int16_t)rxData[0]) << 8) | rxData[1];
      _gycounts = (int16_t)(((int16_t)rxData[2]) << 8) | rxData[3];
      _gzcounts = (int16_t)(((int16_t)rxData[4]) << 8) | rxData[5];
      // EXT_SENS_DATA_00
      fReadMPU9250 ( 6 , EXT_SENS_DATA_00 );
      _hxcounts = ((int16_t)rxData[1] << 8) | rxData[0] ;
      _hycounts = ((int16_t)rxData[3] << 8) | rxData[2] ;;
      _hzcounts = ((int16_t)rxData[5] << 8) | rxData[4] ;;
      //
      /// no biases applied just scale factor
      _ax = (float)_axcounts * _accelScale;
      _ay = (float)_aycounts * _accelScale;
      _az = (float)_azcounts * _accelScale;
      //
      _gx = (float)_gxcounts * _gyroScale;
      _gy = (float)_gycounts * _gyroScale;
      _gz = (float)_gzcounts * _gyroScale;
      //
      _hx = (float)_hxcounts * _magScaleX;
      _hy = (float)_hycounts * _magScaleY;
      _hz = (float)_hzcounts * _magScaleZ;
      ////
      // Serial.print ( "ax = " );
      // Serial.print ( _ax * 1000.0f );
      // Serial.print ( " ay = " );
      // Serial.print ( _ay * 1000.0f );
      // Serial.print ( " az = " );
      // Serial.print ( _az * 1000.0f );
      // Serial.print ( " gx = " );
      // Serial.print ( _gx * 1000.0f );
      // Serial.print ( " gy = " );
      // Serial.print ( _gy * 1000.0f );
      // Serial.print ( " gz = " );
      // Serial.print ( _gz * 1000.0f );
      // Serial.print( " deg/s" );
      // Serial.print ( " hx = " );
      // Serial.print ( _hx );
      // Serial.print ( " hy = " );
      // Serial.print ( _hy );
      // Serial.print ( " hz = " );
      // Serial.print ( _hz );
      // Serial.print ( " uT" );
      // Serial.println();
      // h value multiplied by 10 converts to mG from uT
      MahonyQuaternionUpdate ( _ax, _ay, _az, _gx, _gy, _gz, _hx * 10.0f, _hy * 10.0f, _hz * 10.0f );
      // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
      // In this coordinate system, the positive z-axis is down toward Earth.
      // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
      // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
      // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
      // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
      // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
      // applied in the correct order which for this configuration is yaw, pitch, and then roll.
      // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
      float yaw   = atan2f(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
      float pitch = -asinf(2.0f * (q[1] * q[3] - q[0] * q[2]));
      float roll  = atan2f(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
      pitch *= 180.0f / PI;
      yaw   *= 180.0f / PI;
      yaw   += 13.13f; // Declination
      roll  *= 180.0f / PI;
      TimePast = TimeNow;
      Serial.print ( " Roll: ");
      Serial.print ( roll, 6 );
      Serial.print ( " Pitch: " );
      Serial.print ( pitch, 6 );
      Serial.print ( " Yaw: " );
      Serial.println ( yaw, 6 );
    }
    else
    {
      Serial.print ( "WHO AM I FAILED!! ");
      Serial.print ( "MPU9250 OK = ");
      Serial.print ( MPU9250_OK );
      Serial.print ( " AK8963 OK = " );
      Serial.println ( AK8963_OK );
    }
    // xLastWakeTime = xTaskGetTickCount();
  }
  vTaskDelete(NULL);
} // void fGetIMU( void *pvParameters )
///////////////////////////////////////////
void setup()
{
    eg = xEventGroupCreate();
//   Serial.begin( SerialDataBits );
  xTaskCreatePinnedToCore ( fGetIMU, "v_getIMU", TaskStack30K, NULL, Priority4, NULL, TaskCore0 );
}

void fReadMPU9250 ( uint8_t byteReadSize, uint8_t addressToRead )
{
  esp_err_t intError;
  spi_transaction_t trans_desc;
//   trans_desc = { };
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
void fWriteSPIdata8bits( uint8_t address, uint8_t DataToSend)
{
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
} // void fSendSPI( uint8_t count, uint8_t address, uint8_t DataToSend)
///////////////////////////////////////////////////////////////////////
void fReadAK8963( uint8_t subAddress, uint8_t count )
{
  // set slave 0 to the AK8963 and set for read
  fWriteSPIdata8bits ( I2C_SLV0_ADDR, AK8963_I2C_ADDR | I2C_READ_FLAG );
  // set the register to the desired AK8963 sub address
  fWriteSPIdata8bits ( I2C_SLV0_REG, subAddress );
  // enable I2C and request the bytes
  fWriteSPIdata8bits ( I2C_SLV0_CTRL, I2C_SLV0_EN | count );
  // fwriteMUP9250register ( I2C_SLV0_CTRL, 0x80 );
  vTaskDelay ( 1 );
  
} // fReadAK8963(uint8_t subAddress, uint8_t count, uint8_t* dest)
////////////////////////////////////////////////////////////////////////////////////////////
void fWrite_AK8963 ( uint8_t subAddress, uint8_t dataAK8963 )
{
  fWriteSPIdata8bits ( I2C_SLV0_ADDR, AK8963_I2C_ADDR );
  fWriteSPIdata8bits ( I2C_SLV0_REG, subAddress );
  fWriteSPIdata8bits ( I2C_SLV0_DO, dataAK8963 );
  fWriteSPIdata8bits ( I2C_SLV0_CTRL, I2C_SLV0_EN | (uint8_t)1) ;
  //
  vTaskDelay ( 1 );
} // void fWrite_AK8963 ( uint8_t subAddress, uint8_t dataAK8963 )
/////////////////////////////////////////////////////////////////////////////////////////////
void fwriteMUP9250register ( uint8_t addr, uint8_t sendData )
{
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
  float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
  float norm;
  float hx, hy, bx, bz;
  float vx, vy, vz, wx, wy, wz;
  float ex, ey, ez;
  float pa, pb, pc;
  // Auxiliary variables to avoid repeated arithmetic
  float q1q1 = q1 * q1;
  float q1q2 = q1 * q2;
  float q1q3 = q1 * q3;
  float q1q4 = q1 * q4;
  float q2q2 = q2 * q2;
  float q2q3 = q2 * q3;
  float q2q4 = q2 * q4;
  float q3q3 = q3 * q3;
  float q3q4 = q3 * q4;
  float q4q4 = q4 * q4;
  // Normalise accelerometer measurement
  norm = sqrt(ax * ax + ay * ay + az * az);
  if (norm == 0.0f) return; // handle NaN
  norm = 1.0f / norm;        // use reciprocal for division
  ax *= norm;
  ay *= norm;
  az *= norm;
  // Normalise magnetometer measurement
  norm = sqrt(mx * mx + my * my + mz * mz);
  if (norm == 0.0f) return; // handle NaN
  norm = 1.0f / norm;        // use reciprocal for division
  mx *= norm;
  my *= norm;
  mz *= norm;
  // Reference direction of Earth's magnetic field
  hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
  hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
  bx = sqrt((hx * hx) + (hy * hy));
  bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);
  // Estimated direction of gravity and magnetic field
  vx = 2.0f * (q2q4 - q1q3);
  vy = 2.0f * (q1q2 + q3q4);
  vz = q1q1 - q2q2 - q3q3 + q4q4;
  wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
  wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
  wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);
  // Error is cross product between estimated direction and measured direction of gravity
  ex = (ay * vz - az * vy) + (my * wz - mz * wy);
  ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
  ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
  if (Ki > 0.0f)
  {
    eInt[0] += ex;      // accumulate integral error
    eInt[1] += ey;
    eInt[2] += ez;
  }
  else
  {
    eInt[0] = 0.0f;     // prevent integral wind up
    eInt[1] = 0.0f;
    eInt[2] = 0.0f;
  }
  // Apply feedback terms
  gx = gx + Kp * ex + Ki * eInt[0];
  gy = gy + Kp * ey + Ki * eInt[1];
  gz = gz + Kp * ez + Ki * eInt[2];
  // Integrate rate of change of quaternion
  pa = q2;
  pb = q3;
  pc = q4;
  q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
  q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
  q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
  q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);
  // Normalise quaternion
  norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
  norm = 1.0f / norm;
  q[0] = q1 * norm;
  q[1] = q2 * norm;
  q[2] = q3 * norm;
  q[3] = q4 * norm;
} // void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
//////////////////////////////////////////////