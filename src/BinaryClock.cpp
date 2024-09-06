#include "Logic.h"
#include <Arduino.h>
#include <BinaryClockCore.h>
#include <BinaryClock.h>

const std::string BinaryClockModule::name()
{
    return "BinaryClock";
}

const std::string BinaryClockModule::version()
{
    return MAIN_Version;
}

void BinaryClockModule::setup()
{
    binaryClockCore.setup();
    setBrightness(ParamBIN_StartBrightnessFixedValue);
}

void BinaryClockModule::loop()
{
    binaryClockCore.loop(openknxLogic.Now(), openknxLogic.timeValid());
}

void BinaryClockModule::setPower(bool on)
{
    binaryClockCore.setPower(on);
    KoBIN_SwitchStatus.value(on, DPT_Switch);
}

void BinaryClockModule::setBrightness(uint8_t percent)
{
    KoBIN_BrightnessStatus.value(percent, DPT_Scaling);
    uint8_t ledOnBrightness = SCALING_MAX * percent / 100;
    logDebugP("LED on brightness changed: %d", ledOnBrightness);

    uint8_t ledOffBrightness;
    switch (ParamBIN_InactiveBitsBrightness)
    {
        case 0:
            ledOffBrightness = SCALING_MAX * ParamBIN_InactiveBitsBrightnessFixedValue / 100;
            break;

        case 2:
            ledOffBrightness = ledOnBrightness * ParamBIN_InactiveBitsBrightnessPercentActive / 100;
            break;
    }
    logDebugP("LED off brightness changed: %d", ledOffBrightness);

    _ledOnBrightnessPercent = percent;
    binaryClockCore.setBrightness(ledOnBrightness, ledOffBrightness);
}

void BinaryClockModule::processInputKo(GroupObject &ko)
{
    bool switchedOn;

    uint16_t lAsap = ko.asap();
    switch (lAsap)
    {
        case BIN_KoSwitch:
            switchedOn = KoBIN_Switch.value(DPT_Switch);
            setPower(switchedOn);
            logDebugP("Switch changed: %d", switchedOn);
            break;

        case BIN_KoDimAbsolute:
            setBrightness(KoBIN_DimAbsolute.value(DPT_Scaling));
            break;

        case BIN_KoDimRelative:
            bool increaseBrightness = ko.value(Dpt(3, 7, 0));
            uint8_t stepCode = ko.value(Dpt(3, 7, 1));
            uint8_t stepPercentSize = 100 / (2 ^ (stepCode - 1));

            if (increaseBrightness)
                setBrightness(std::min(_ledOnBrightnessPercent + stepPercentSize, 100));
            else
                setBrightness(std::max(_ledOnBrightnessPercent - stepPercentSize, 0));

            break;
    }
}

uint16_t BinaryClockModule::flashSize()
{
    // Version + switchedOn + ledOnBrightnessPercent
    return 1 + 1 + 1;
}

void BinaryClockModule::readFlash(const uint8_t *data, const uint16_t size)
{
    if (size == 0) // first call - without data
        return;

    byte version = openknx.flash.readByte();
    if (version != 1) // version unknown
    {
        logDebugP("Wrong version of flash data: version %d", version);
        return;
    }

    bool switchedOn = openknx.flash.readByte();
    binaryClockCore.setPower(switchedOn);
    KoBIN_SwitchStatus.valueNoSend(switchedOn, DPT_Switch);
    logDebugP("switchedOn read from flash: %d", switchedOn);

    uint8_t lastLedOnBrightnessPercent = openknx.flash.readByte();
    logDebugP("lastLedOnBrightnessPercent read from flash: %d", lastLedOnBrightnessPercent);

    if (ParamBIN_StartBrightness == 2)
        setBrightness(lastLedOnBrightnessPercent);
    else
        setBrightness(ParamBIN_StartBrightnessFixedValue);
}

void BinaryClockModule::writeFlash()
{
    bool switchedOn = binaryClockCore.getPower();

    openknx.flash.writeByte(1); // Version
    openknx.flash.writeByte(switchedOn);
    openknx.flash.writeByte(_ledOnBrightnessPercent);
}

void BinaryClockModule::printDateTime(time_t t)
{
    logTraceP("%d-%.2d-%.2d %.2d:%.2d:%.2d", year(t), month(t), day(t), hour(t), minute(t), second(t));
}

BinaryClockModule openknxBinaryClockModule;