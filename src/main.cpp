#include "BinaryClock.h"
#include "Logic.h"
#include "FileTransferModule.h"

void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);
    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, openknxBinaryClockModule);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

void loop()
{
	openknx.loop();
}