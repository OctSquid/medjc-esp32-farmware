#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>
#include <stddef.h>

#include <PacketSerial.h>

#include <Adc.h>
#include <CommandParser.h>
#include <Version.h>

// Function declarations
class CommandHandler
{
private:
    PacketSerial *packetSerial;
    ADC *adc;
public:
    CommandHandler(PacketSerial *packetSerial, ADC *adc);
    void handleCommand(const Command &cmd);
    virtual void sendErr(uint8_t errCode);
    virtual void sendResponse(uint8_t cmd, const uint8_t *data, size_t length);
    void handleGetVersion();
    void handleGetBaseVoltage();
    void handleGetConnections();
    void handleGetME();
    void handleGetSME();
};

#endif // COMMAND_HANDLER_H
