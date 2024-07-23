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

/**
 * @brief Command handler class.
 */
class CommandHandler
{
private:
    static PacketSerial *_packetSerial; /**< Pointer to the PacketSerial object. */

public:
    static uint8_t testBuffer[64]; /**< Buffer for testing purposes. */

    CommandHandler() = delete;

    /**
     * @brief Initializes the command handler.
     * ! This function must be called before any other functions in this class.
     * @param packetSerial Pointer to the PacketSerial object.
     */
    static void init(PacketSerial *packetSerial);

    /**
     * @brief Handles a command.
     * @param cmd The command to handle.
     */
    static void handleCommand(const Command &cmd);

    /**
     * @brief Sends an error response.
     * @param errCode The error code.
     */
    static void inline sendErr(uint8_t errCode)
    {
        uint8_t errResponse[3] = {SERTX, errCode, EERTX};
#ifdef PIO_UNIT_TESTING
        memcpy(testBuffer, errResponse, sizeof(errResponse));
#else
        _packetSerial->send(errResponse, sizeof(errResponse));
#endif
    };

    /**
     * @brief Sends a response.
     * @param cmd The command.
     * @param data The data to send.
     * @param length The length of the data.
     */
    static void inline sendResponse(uint8_t cmd, uint16_t id, const uint8_t *data, size_t length)
    {
        uint8_t response[64] = {STX, cmd, highByte(id), lowByte(id)};
        if (length > 0)
        {
            memcpy(response + 4, data, length);
        }
        response[length + 4] = ETX;
#ifdef PIO_UNIT_TESTING
        memcpy(testBuffer, response, length + 5);
#else
        _packetSerial->send(response, length + 5);
#endif
    };

    static void handlePing(uint16_t id);
    static void handleGetVersion(uint16_t id);
    static void handleGetBaseVoltage(uint16_t id);
    static void handleGetConnections(uint16_t id);
    static void handleGetME(uint16_t id);
    static void handleGetSME(uint16_t id);
    static void handleStartPRM(uint16_t id);
    static void handleStopPRM(uint16_t id);
    static void handleSetPRRate(uint16_t id, const uint8_t *params, size_t length);
    static void handleGetPRRate(uint16_t id);
    static void handleGetPR(uint16_t id);
};

#endif // COMMAND_HANDLER_H
