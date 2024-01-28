#ifndef IS31FL3265A_h
#define IS31FL3265A_h

#include "Arduino.h"
#include "I2CDev.h"
#include <Wire.h>

/* IS31FL3265A registers
*  https://www.lumissil.com/assets/pdf/core/IS31FL3265A_DS.pdf
*/
#define IS31FL3265A_CONFIG         0x00
#define IS31FL3265A_GCC            0x01
#define IS31FL3265A_RESET          0x3F

#define IS31FL3265A_DC_PWM_1       0x1A
#define IS31FL3265A_DC_PWM_2       0x1B
#define IS31FL3265A_DC_PWM_3       0x1C

#define IS31FL3265A_SCALING_OFFSET 0x02
#define IS31FL3265A_SCALING_UPDATE 0x36

class IS31FL3265A
{
    public: 
        IS31FL3265A(I2Cdev* i2c_bus, uint8_t address);
        void init(bool slave, uint8_t globalCurrentMax);
        void reset();
        void disable();
        void enable();
        void changeScaling(uint8_t outNum, uint8_t newScaling);
        void applyScaling();
    private:
        I2Cdev* _i2c_bus;
        uint8_t _address = 0;
};

#endif