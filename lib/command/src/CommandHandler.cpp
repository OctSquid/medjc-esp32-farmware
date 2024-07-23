#include <CommandHandler.h>

PacketSerial *CommandHandler::_packetSerial = nullptr;
uint8_t CommandHandler::testBuffer[64];

void CommandHandler::init(PacketSerial *packetSerial)
{
#ifdef PIO_UNIT_TESTING
    memset(testBuffer, 0, sizeof(testBuffer));
#endif
    _packetSerial = packetSerial;
}

void CommandHandler::handleCommand(const Command &cmd)
{
    switch (cmd.type)
    {
    case CMD_PING:
        handlePing(cmd.id);
        break;
    case CMD_GET_VERSION:
        handleGetVersion(cmd.id);
        break;
    case CMD_GET_BASE_VOLTAGE:
        handleGetBaseVoltage(cmd.id);
        break;
    case CMD_GET_CONNECTIONS:
        handleGetConnections(cmd.id);
        break;
    case CMD_GET_ME:
        handleGetME(cmd.id);
        break;
    case CMD_GET_SME:
        handleGetSME(cmd.id);
        break;
    case CMD_START_PRM:
        handleStartPRM(cmd.id);
        break;
    case CMD_STOP_PRM:
        handleStopPRM(cmd.id);
        break;
    case CMD_SET_PRR:
        handleSetPRRate(cmd.id, cmd.params, cmd.paramLength);
        break;
    case CMD_GET_PRR:
        handleGetPRRate(cmd.id);
        break;
    case CMD_GET_PR:
        handleGetPR(cmd.id);
        break;
    default:
        sendErr(0x11);
        break;
    }
}

void CommandHandler::handlePing(uint16_t id)
{
    sendResponse(CMD_PING, id, {}, 0);
}

void CommandHandler::handleGetVersion(uint16_t id)
{
    uint8_t versionData[3] = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};
    sendResponse(CMD_GET_VERSION, id, versionData, 3);
}

void CommandHandler::handleGetBaseVoltage(uint16_t id)
{
    int16_t value = ADC::readBaseVoltage();
    uint8_t data[2] = {highByte(value), lowByte(value)};
    sendResponse(CMD_GET_BASE_VOLTAGE, id, data, 2);
}

void CommandHandler::handleGetConnections(uint16_t id)
{
    uint8_t data[6];
    for (int i = 0; i < 4; ++i)
    {
        data[i] = ADC::isConnected(i) ? 0x01 : 0x00;
    }
    sendResponse(CMD_GET_CONNECTIONS, id, data, 4);
}

void CommandHandler::handleGetME(uint16_t id)
{
    uint8_t data[8];
    for (int i = 0; i < 4; ++i)
    {
        int16_t value;
        if (ADC::isConnected(i))
        {
            value = ADC::readME(i);
        }
        else
        {
            value = 0;
        }
        data[2 * i] = highByte(value);
        data[2 * i + 1] = lowByte(value);
    }
    sendResponse(CMD_GET_ME, id, data, 8);
}

void CommandHandler::handleGetSME(uint16_t id)
{
    uint8_t data[8];
    for (int i = 0; i < 4; ++i)
    {
        int16_t value;
        if (ADC::isConnected(i))
        {
            value = ADC::readSME(i);
        }
        else
        {
            value = 0;
        }
        data[2 * i] = highByte(value);
        data[2 * i + 1] = lowByte(value);
    }
    sendResponse(CMD_GET_SME, id, data, 8);
};

void CommandHandler::handleStartPRM(uint16_t id)
{
    PollingManager::start();
    sendResponse(CMD_START_PRM, id, {}, 0);
};

void CommandHandler::handleStopPRM(uint16_t id)
{
    PollingManager::stop();
    sendResponse(CMD_STOP_PRM, id, {}, 0);
};

void CommandHandler::handleSetPRRate(uint16_t id, const uint8_t *params, size_t length)
{
    int16_t rate = (params[0] << 8) | params[1];
    PollingManager::setRate(rate);
    sendResponse(CMD_SET_PRR, id, {}, 0);
};

void CommandHandler::handleGetPRRate(uint16_t id)
{
    int16_t rate = PollingManager::getRate();
    uint8_t data[2] = {highByte(rate), lowByte(rate)};
    sendResponse(CMD_GET_PRR, id, data, 2);
};

void CommandHandler::handleGetPR(uint16_t id)
{
    PollingManager::sendReport(id);
};
