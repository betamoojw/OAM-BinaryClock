#include "IS31FL3265A.h"
#include "I2CDev.h"

IS31FL3265A::IS31FL3265A(I2Cdev* i2c_bus, uint8_t address)
{
    _i2c_bus = i2c_bus;
    _address = address;
}

void IS31FL3265A::init(bool slave, uint8_t globalCurrentMax)
{
    // set master/slave mode (slave has to be initialized first!)
    // current multiplied (CM) enabled, resistor 6.8 k ohm
    _i2c_bus->writeByte(_address, IS31FL3265A_CONFIG, slave ? 0b00100110 : 0b00100100);

    // set DC mode for all outputs
    _i2c_bus->writeByte(_address, IS31FL3265A_DC_PWM_1, 0b01110111);
    _i2c_bus->writeByte(_address, IS31FL3265A_DC_PWM_2, 0b01110111);
    _i2c_bus->writeByte(_address, IS31FL3265A_DC_PWM_3, 0b01110111);

    // set maximum global current
    _i2c_bus->writeByte(_address, IS31FL3265A_GCC, globalCurrentMax);
}

void IS31FL3265A::reset() // return to default registers/conditions
{
    _i2c_bus->writeByte(_address, IS31FL3265A_RESET, 0x00);
}

void IS31FL3265A::disable()
{
    uint8_t temp = _i2c_bus->readByte(_address, IS31FL3265A_CONFIG);
    _i2c_bus->writeByte(_address, IS31FL3265A_CONFIG, temp & ~(0x01)); // clear bit 0 to shut down
}

void IS31FL3265A::enable()
{
    uint8_t temp = _i2c_bus->readByte(_address, IS31FL3265A_CONFIG);
    _i2c_bus->writeByte(_address, IS31FL3265A_CONFIG, temp | 0x01); // set bit 0 to enable
}

void IS31FL3265A::changeScaling(uint8_t outNum, uint8_t newScaling) {
    _i2c_bus->writeByte(_address, IS31FL3265A_SCALING_OFFSET + outNum - 1, newScaling);
}

void IS31FL3265A::applyScaling()
{
    _i2c_bus->writeByte(_address, IS31FL3265A_SCALING_UPDATE, 0x00);
}