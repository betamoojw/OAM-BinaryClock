#include "IS31FL3265A.h"

#define I2C_BUS Wire1
#define SDA_PIN 2
#define SCL_PIN 3
#define SDB_PIN 4

#define IS31FL3265A_ADDR_1 0x40
#define IS31FL3265A_ADDR_2 0x43

#define GLOBAL_CURRENT_MAX 22 // = ~ 5.16 mA with CM=1 and 6.8 k ohm
#define SCALING_MAX 255

#define LEDS_SECOND_COUNT 6
#define LEDS_MINUTE_COUNT 6
#define LEDS_HOUR_COUNT 5
#define LEDS_DAY_COUNT 5
#define LEDS_MONTH_COUNT 4
#define LEDS_YEAR_COUNT 7

class BinaryClockCore
{
  public:
    void setup();
    void loop(tm &tm_local, bool timeValid);

    void setPower(bool on);
    void setBrightness(uint8_t ledOnBrightness, uint8_t ledOffBrightness);

    bool getPower();

  private:
    I2Cdev _i2c = I2Cdev(&I2C_BUS);
    IS31FL3265A _ledDriver1 = IS31FL3265A(&_i2c, IS31FL3265A_ADDR_1);
    IS31FL3265A _ledDriver2 = IS31FL3265A(&_i2c, IS31FL3265A_ADDR_2);
    bool _ledDriversEnabled = false;

    const uint8_t LEDS_SECOND[6] = {1, 2, 3, 4, 5, 6};
    const uint8_t LEDS_MINUTE[6] = {7, 8, 9, 10, 11, 12};
    const uint8_t LEDS_HOUR[5] = {13, 14, 15, 16, 17};
    const uint8_t LEDS_DAY[5] = {9, 8, 7, 6, 5};
    const uint8_t LEDS_MONTH[4] = {4, 3, 2, 1};
    const uint8_t LEDS_YEAR[7] = {10, 11, 12, 13, 14, 15, 16};

    int _currentSecond = -1;
    int _currentMinute = -1;
    int _currentHour = -1;
    int _currentDay = -1;
    int _currentMonth = -1;
    int _currentYear = -1;

    bool _switchedOn = false;
    uint8_t _ledOnBrightness = SCALING_MAX;
    uint8_t _ledOffBrightness = 0;

    void updateLeds(IS31FL3265A ledDriver, const uint8_t leds[], uint8_t count, uint8_t value);
    void updateAllLeds();
};

extern BinaryClockCore binaryClockCore;