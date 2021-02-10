/* 
    This task is for aquiring raw data and saving it to a sd card
    as well as making the data available to the rest of the program.
*/

#pragma once

#include "daqPacket.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include <string.h>

void daqLoop (void * pvParameters) {
    struct DAQPacket daqPacket;
    daqPacket.testValue = 0;

    // Test SPI code

    // setup SPI bus

    ESP_LOGI("spi_init", "setup bus");
    spi_bus_config_t buscfg;
    buscfg.mosi_io_num = 23;
    buscfg.miso_io_num = 19;
    buscfg.sclk_io_num = 18;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    esp_err_t ret;
    ret=spi_bus_initialize(SPI3_HOST, &buscfg, 0);
    ESP_ERROR_CHECK(ret);

    // setup each sensor
    ESP_LOGI("spi_init", "setup sensor");
    spi_device_handle_t spi;

    spi_device_interface_config_t devcfg;
    devcfg.clock_speed_hz=100*1000; // clock speed 0.1MHz
    devcfg.mode = 3; // SPI mode
    devcfg.spics_io_num = 5; // CS pin
    devcfg.queue_size = 1;
    devcfg.cs_ena_pretrans = 2;
    devcfg.cs_ena_posttrans = 0;
    devcfg.command_bits = 8;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;
    devcfg.duty_cycle_pos = 128; // 50% duty cycle 
    devcfg.input_delay_ns = 0;
    
    ret=spi_bus_add_device(SPI3_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    // // read register
    // ESP_LOGI("spi_init", "transaction");
    spi_transaction_t transaction;
    // memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
    // transaction.length = 8; // only sent one bit address read command
    // transaction.rxlength = 8; // reister is 1 byte
    // transaction.cmd = 0b11101010; // Read (0b1) register 106 User Control (0b1101010)
    // transaction.user=(void*)1; //D/C needs to be set to 1
    // ret=spi_device_transmit(spi, &transaction);  //Transmit!
    // assert(ret==ESP_OK); //Should have had no issues.
    // ESP_LOGI("spi_init", "Byte recived: %i\n", transaction.tx_data[0]);

    // write register
    // ESP_LOGI("spi_init", "write data");
    // memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
    // transaction.length= 8 * 2; // sent two bit address write command
    // transaction.rxlength = 0; // reister is one byte
    // transaction.cmd = 0b0110101000010000; // Write (0b0) register 106 User Control (0b1101010) and data (0b00010000)
    // transaction.user=(void*)1; //D/C needs to be set to 1
    // ret=spi_device_transmit(spi, &transaction);  //Transmit!
    // assert(ret==ESP_OK); //Should have had no issues.

    // // read again
    // memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
    // transaction.length=8; // only sent one bit address read command
    // transaction.rxlength=8; // reister is 1 byte
    // transaction.cmd = 0b11101010; // Read (0b1) register 106 User Control (0b1101010)
    // transaction.user=(void*)1; //D/C needs to be set to 1
    // ret=spi_device_transmit(spi, &transaction);  //Transmit!
    // assert(ret==ESP_OK); //Should have had no issues.
    // ESP_LOGI("spi_init", "Byte recived: %i\n", transaction.tx_data[0]);
    // // End test SPI code

    // // read register
    // ESP_LOGI("spi_init", "transaction");
    // memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
    // transaction.length = 8; // only sent one bit address read command
    // transaction.rxlength = 8; // reister is 1 byte
    // transaction.cmd = 0b11110101; // Read (0b1) register 117 User Control (0b1110101)
    // transaction.user=(void*)1; //D/C needs to be set to 1
    // ret=spi_device_transmit(spi, &transaction);  //Transmit!
    // assert(ret==ESP_OK); //Should have had no issues.
    // ESP_LOGI("spi_init", "Byte recived: %i\n", transaction.tx_data[0]);

    for (;;) { // loop  forever

        // test code
        daqPacket.testValue++;
        vTaskDelay(10);

        *(struct DAQPacket*)pvParameters = daqPacket; // send data in packet back to main

        ESP_LOGI("spi_init", "write data");
        memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
        transaction.length= 8; // sent two bit address write command
        transaction.rxlength = 0; // reister is one byte
        transaction.cmd = 0b01101010; // Write (0b0) register 106 User Control (0b1101010)
        uint8_t data = 0b00010000; // data (0b00010000)
        transaction.tx_buffer = &data;
        transaction.user=(void*)1; //D/C needs to be set to 1
        ret=spi_device_transmit(spi, &transaction);  //Transmit!
        assert(ret==ESP_OK); //Should have had no issues.

        // read register
        ESP_LOGI("spi_init", "transaction");
        memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
        transaction.length = 8; // only sent one bit address read command
        transaction.rxlength = 8; // reister is 1 byte
        data = 0x00; // dont send anything
        transaction.tx_buffer = &data;
        transaction.cmd = 0b11110101; // Read (0b1) register 117 User Control (0b1110101)
        transaction.user=(void*)1; //D/C needs to be set to 1
        ret=spi_device_transmit(spi, &transaction);  //Transmit!
        assert(ret==ESP_OK); //Should have had no issues.
        // uint8_t readData = *(uint8_t*)(transaction.rx_buffer);
        // ESP_LOGI("spi_init", "Byte recived: %i\n", readData);
        
        // read register
        ESP_LOGI("spi_init", "transaction");
        memset(&transaction, 0, sizeof(transaction)); // zero out the transaction
        transaction.length = 8; // only sent one bit address read command
        transaction.rxlength = 8; // reister is 1 byte
        data = 0x00; // dont send anything
        transaction.tx_buffer = &data;
        transaction.cmd = 0; // Read (0b1) register 117 User Control (0b1110101)
        transaction.user=(void*)1; //D/C needs to be set to 1
        ret=spi_device_transmit(spi, &transaction);  //Transmit!
        assert(ret==ESP_OK); //Should have had no issues.
    }
}