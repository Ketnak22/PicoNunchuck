#pragma once

#include <stdio.h>
#include <array>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define NUNCHUK_ADDRESS 0x52

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

class Nunchuck
{
private:
    // Calibration accelerometer values, depends on your Nunchuk
    int ACCEL_X_ZERO = 512;
    int ACCEL_Y_ZERO = 512;
    int ACCEL_Z_ZERO = 512;

    // Calibration joystick values
    int JOYSTICK_X_ZERO = 127;
    int JOYSTICK_Y_ZERO = 128;


    std::array<uint8_t, 6> data;

    // Read identifaction number of device
    std::array<uint8_t, 4> read_ident()
    {
        uint8_t reg = 0xFA;
        uint8_t buf[6];

        i2c_write_blocking(I2C_PORT, NUNCHUK_ADDRESS, &reg, 1, false);
        sleep_ms(5);
        i2c_read_blocking(I2C_PORT, NUNCHUK_ADDRESS, buf, 6, false);

        std::array<uint8_t, 4> ident = {buf[2], buf[3], buf[4], buf[5]};

        return ident;
    }

    // Check if identifaction number is the one of Nunchuck
    bool isNunchuck()
    {
        auto ident = read_ident();
        return ident[0] == 0xA4 && ident[1] == 0x20 && ident[2] == 0x00 && ident[3] == 0x00;
    }
public:
    // Initialize the Nunchuck without encryption
    void init()
    {
        sleep_ms(1000);

        uint8_t reg1[2] = {0xF0, 0x55};
        uint8_t reg2[2] = {0xFB, 0x00};

        if (i2c_write_blocking(I2C_PORT, NUNCHUK_ADDRESS, reg1, 2, false) < 0)
        {
            while (true)
            {
                printf("Cannot initialize device - check the wiring!\n");
            }
        }
        sleep_ms(5);
        i2c_write_blocking(I2C_PORT, NUNCHUK_ADDRESS, reg2, 2, false);

        if (this->isNunchuck())
        {
            while (true)
            {
                printf("This device is not the Nunchuck!\n");
            }
        }
    }

    // Retrieve raw measurements data
    bool read()
    {
        uint8_t reg = 0x00;
        std::array<uint8_t, 6> buf;

        if (i2c_write_blocking(I2C_PORT, NUNCHUK_ADDRESS, &reg, 1, false) < 0) {
            return false;
        }
        sleep_ms(5);
        if(i2c_read_blocking(I2C_PORT, NUNCHUK_ADDRESS, buf.data(), 6, false) < 0) {
            return false;
        }

        data = buf;
        return true;
    }

    // Read chunk of data from Nunchuck
    std::array<uint8_t, 6> raw_data()
    {
        return data;
    }

    // Checks current state of button Z 
    bool buttonZ()
    {
        return (~data[5] >> 0) & 1;
    }

    // Check current state of button C
    bool buttonC()
    {
        return (~data[5] >> 1) & 1;
    }

    // Retrieves the raw X-value of the joystick
    uint8_t joystickX_raw()
    {
        return data[0];
    }

    // Retrieves the raw Y-value of the joystick
    uint8_t joystickY_raw()
    {
        return data[1];
    }

    // Retrieves the calibrated X-value of the joystick
    int16_t joystickX()
    {
        return (int16_t)this->joystickY_raw() - (int16_t)JOYSTICK_X_ZERO;
    }

    // Retrieves the calibrated Y-value of the joystick
    int16_t joystickY()
    {
        return (int16_t)this->joystickY_raw() - (int16_t)JOYSTICK_Y_ZERO;
    }

    // Retrieves the raw X-value of the accelerometer
    uint16_t accelX_raw()
    {
        return ((uint16_t)data[2] << 2) | ((data[5] >> 2) & 3);
    }

    // Retrieves the raw Y-value of the accelerometer
    uint16_t accelY_raw()
    {
        return ((uint16_t)data[3] << 2) | ((data[5] >> 4) & 3);
    }

    // Retrieves the raw Z-value of the accelerometer
    uint16_t accelZ_raw()
    {
        return ((uint16_t)data[4] << 2) | ((data[5] >> 6) & 3);
    }

    // Retrieves the calibrated X-value of the accelerometer
    int16_t accelX()
    {
        return (int16_t)this->accelX_raw() - (int16_t)ACCEL_X_ZERO;
    }

    // Retrieves the calibrated Y-value of the accelerometer
    int16_t accelY()
    {
        return (int16_t)this->accelY_raw() - (int16_t)ACCEL_Y_ZERO;
    }

     // Retrieves the calibrated Z-value of the accelerometer
    int16_t accelZ()
    {
        return (int16_t)this->accelZ_raw() - (int16_t)ACCEL_Z_ZERO;
    }

    // Print all values 
    void print() {
        printf("Accel X:%d Y:%d Z:%d  Joystick X:%d Y:%d  Button C:%d  Button Z:%d\n", this->accelX(), this->accelY(), this->accelZ(), this->joystickX(), this->joystickY(), this->buttonC(), this->buttonZ());
    }
};