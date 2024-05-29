#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>
#include <stddef.h>

#include <PacketSerial.h>

#include <Adc.h>
#include <CommandParser.h>
#include <PollingManager.h>
#include <Version.h>

class PollingManager;
// Function declarations
class CommandHandler
{
private:
    static PacketSerial *_packetSerial;
public:
    CommandHandler() = delete;
    static void init(PacketSerial *packetSerial);
    static void handleCommand(const Command &cmd);
    static void sendErr(uint8_t errCode);
    static void sendResponse(uint8_t cmd, const uint8_t *data, size_t length);
    static void handleGetVersion();
    static void handleGetBaseVoltage();
    static void handleGetConnections();
    static void handleGetME();
    static void handleGetSME();
    static void handleStartPRM();
    static void handleStopPRM();
    static void handleSetPRRate(const uint8_t *params, size_t length);
    static void handleGetPRRate();
    static void handleGetPR();
};

#endif // COMMAND_HANDLER_H
