#include <Arduino.h>
#include <BinaryClockCore.h>
#include <TimeLib.h>

void BinaryClockCore::setup()
{
    pinMode(SDB_PIN, OUTPUT);
    setPower(true);

#ifdef ESP32
    I2C_BUS.begin(SDA_PIN, SCL_PIN, 400000);
#else
    I2C_BUS.setSDA(SDA_PIN);
    I2C_BUS.setSCL(SCL_PIN);
    I2C_BUS.begin();          // Set master mode
    I2C_BUS.setClock(400000); // I2C frequency at 400 kHz
#endif

    // Serial.println("Scan for I2C devices:");
    // _i2c.I2Cscan(); // should detect both IS31FL3265A
    // delay(1000);

    _ledDriver2.init(true, GLOBAL_CURRENT_MAX); // slave needs to be initialized first
    _ledDriver1.init(false, GLOBAL_CURRENT_MAX);
}

void BinaryClockCore::loop(tm &currentTime, bool timeValid)
{
    if (!timeValid)
    {
        // show sad face if current time is invalid
        currentTime.tm_year = 0b110011;
        currentTime.tm_mon = -1; // 1 will be added
        if (currentTime.tm_sec % 2 == 1)
            currentTime.tm_mday = 0b001100;
        else
            currentTime.tm_mday = 0b000000;
        currentTime.tm_hour = 0b000000;
        currentTime.tm_min = 0b011110;
        currentTime.tm_sec = 0b100001;
    }

    // if (millis() % 1000 == 0)
    //     Serial.println(&currentTime, "%A, %B %d %Y %H:%M:%S");

    if (_currentSecond != currentTime.tm_sec)
    {
        _currentSecond = currentTime.tm_sec;
        updateLeds(_ledDriver1, LEDS_SECOND, LEDS_SECOND_COUNT, _currentSecond);
    }

    if (_currentMinute != currentTime.tm_min)
    {
        _currentMinute = currentTime.tm_min;
        updateLeds(_ledDriver1, LEDS_MINUTE, LEDS_MINUTE_COUNT, _currentMinute);
    }

    if (_currentHour != currentTime.tm_hour)
    {
        _currentHour = currentTime.tm_hour;
        updateLeds(_ledDriver1, LEDS_HOUR, LEDS_HOUR_COUNT, _currentHour);
    }

    if (_currentDay != currentTime.tm_mday)
    {
        _currentDay = currentTime.tm_mday;
        updateLeds(_ledDriver2, LEDS_DAY, LEDS_DAY_COUNT, _currentDay);
    }

    if (_currentMonth != currentTime.tm_mon + 1)
    {
        _currentMonth = currentTime.tm_mon + 1; // month is 0-11
        updateLeds(_ledDriver2, LEDS_MONTH, LEDS_MONTH_COUNT, _currentMonth);
    }

    if (_currentYear != currentTime.tm_year % 100)
    {
        _currentYear = currentTime.tm_year % 100;
        updateLeds(_ledDriver2, LEDS_YEAR, LEDS_YEAR_COUNT, _currentYear);
    }

    if (!_ledDriversEnabled)
    {
        _ledDriver1.enable();
        _ledDriver2.enable();

        _ledDriversEnabled = true;
    }
}

void BinaryClockCore::updateLeds(IS31FL3265A ledDriver, const uint8_t leds[], uint8_t count, uint8_t value)
{
    for (byte i = 0; i < count; i++)
    {
        if (bitRead(value, i))
            ledDriver.changeScaling(leds[i], _ledOnBrightness);
        else
            ledDriver.changeScaling(leds[i], _ledOffBrightness);
    }

    ledDriver.applyScaling();
}

void BinaryClockCore::updateAllLeds()
{
    updateLeds(_ledDriver1, LEDS_SECOND, LEDS_SECOND_COUNT, _currentSecond);
    updateLeds(_ledDriver1, LEDS_MINUTE, LEDS_MINUTE_COUNT, _currentMinute);
    updateLeds(_ledDriver1, LEDS_HOUR, LEDS_HOUR_COUNT, _currentHour);
    updateLeds(_ledDriver2, LEDS_DAY, LEDS_DAY_COUNT, _currentDay);
    updateLeds(_ledDriver2, LEDS_MONTH, LEDS_MONTH_COUNT, _currentMonth);
    updateLeds(_ledDriver2, LEDS_YEAR, LEDS_YEAR_COUNT, _currentYear);
}

void BinaryClockCore::setPower(bool on)
{
    if (on == _switchedOn)
        return;

    digitalWrite(SDB_PIN, on ? HIGH : LOW);
    _switchedOn = on;
}

void BinaryClockCore::setBrightness(uint8_t ledOnBrightness, uint8_t ledOffBrightness)
{
    if (_ledOnBrightness == ledOnBrightness &&
        _ledOffBrightness == ledOffBrightness)
        return;

    _ledOnBrightness = ledOnBrightness;
    _ledOffBrightness = ledOffBrightness;

    if (_ledDriversEnabled)
        updateAllLeds();
}

bool BinaryClockCore::getPower()
{
    return _switchedOn;
}

BinaryClockCore binaryClockCore;