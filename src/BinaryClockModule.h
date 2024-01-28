#include "OpenKNX.h"

class BinaryClockModule : public OpenKNX::Module
{
  public:
    void setup() override;
    void loop() override;
    // void processAfterStartupDelay() override;
    void processInputKo(GroupObject &ko) override;

    const std::string name() override;
    const std::string version() override;
    uint16_t flashSize() override;
    void readFlash(const uint8_t *data, const uint16_t size) override;
    void writeFlash() override;

  private:
    void setPower(bool on);
    void setBrightness(uint8_t percent);
    void printDateTime(time_t t);

    uint8_t _ledOnBrightnessPercent = 100;
};

extern BinaryClockModule openknxBinaryClockModule;